# When is garbage collection run?
When you run commands that generate or modify objects, Git evaluates whether housekeeping is required

• Loose Objects Exceed Limit: The number of loose (unpacked) objects in .git/objects/ crosses the gc.auto threshold, which defaults to 6,700 objects.
• Pack Files Exceed Limit: The number of pack files exceeds the gc.autoPackLimit threshold, which defaults to 50 pack files.
