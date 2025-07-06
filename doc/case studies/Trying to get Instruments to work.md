# Background

Trying to use Instruments to profile the app -- as a start to optimisation.
Instruments app was running the program, but without the stack trace.

## Were debug symbols missing?

Initial suspicion was that the debug symbols were missing because I tried:

`dwarfdump [binary]` and the debug section was missing.

But from this stackoverflow thread: https://stackoverflow.com/questions/56688359/debug-symbols-not-included-in-gcc-compiled-c
- macOS compilation leaves the debug information in the .o files and writes a debug map into the binary
- The debug map is stripped when stripping the binary
- Check for the presence of the debug map with: `nm -ap <PATH_TO_BINARY> | grep OSO`

## Picking up which traces

When initialising a trace profile, there is an option to choose reporting threshold - set it to the minimum reporting threshold


## Case Studies

### checkout

Ran `checkout` under the CPU profiling template.
Committed a relatively large file (around 512kb), and checked out to the branch with the large file.

In the initial run, it was 831.57 Mc, with most of it dedicated to running GitObject::read.
```cpp
std::ifstream input_file(paths.string(),
                           std::ios_base::in | std::ios_base::binary);
  vector<char> decompressed_data;

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_decompressor());
  in.push(input_file);
  try {
    boost::iostreams::copy(in, std::back_inserter(decompressed_data));
  }
  ...
  std::string raw(decompressed_data.begin(), decompressed_data.end());
  ...
```

Changing it from a `vector<char>` to a `stringstream` improved performance drastically (~47 Mc):
```cpp
  std::ifstream input_file(paths.string(),
                           std::ios_base::in | std::ios_base::binary);
  std::stringstream decompressed_data;

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_decompressor());
  in.push(input_file);
  try {
    boost::iostreams::copy(in, decompressed_data);
  } catch (const boost::iostreams::zlib_error &e) {
    std::cerr << "Zlib decompression error: " << e.what() << std::endl;
  }

  std::string raw = decompressed_data.str();
```

What made this better?
- Inserts to `vector<char>` were expensive. TODO figure out the mechanics behind this.
- Much cheaper to copy to a stringstream


Low hanging fruit:
- using `boost::trim_right` instead of the substring thing that I was doing in `std::string read_file(const fs::path &filePath, bool remove_newline = false)`. Weightage of `GitObject::find()` reduced from 1.3% -> 0.5%
- Another thing I could try is to reduce the amount of calls to GitObject::read() in the initialise_tree functions. But I've not done it yet.
