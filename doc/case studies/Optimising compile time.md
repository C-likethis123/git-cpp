## Using Perfetto to figure out what's in the build process
- Perfetto: ui.perfetto.dev
- ninjatracing: https://github.com/nico/ninjatracing

How much time is spent on boost vs non boost files:
- non boost:
```sql
select sum(dur) from slice where name not like '%_deps%'
```
23670000000

```sql
select sum(dur) from slice where name like '%_deps%'
```
388555000000

94.25% of the time is spent on compiling boost files

log	91562000000	23.56
url	57465000000	14.79
test	52724000000	13.57
serialization	42221000000	10.87
fiber	26717000000	6.88
locale	22497000000	5.79
wave	22091000000	5.69
program_options	16234000000	4.18
filesystem	11836000000	3.05
iostreams	10248000000	2.64
graph	9931000000	2.56
thread	5229000000	1.35
chrono	3455000000	0.89
json	2650000000	0.68
nowide	2213000000	0.57
contract	1786000000	0.46
type_erasure	1707000000	0.44
coroutine	1575000000	0.41
stacktrace	1445000000	0.37
timer	1301000000	0.33
random	1212000000	0.31
container	1179000000	0.3
context	631000000	0.16
atomic	384000000	0.1
exception	140000000	0.04
date_time	122000000	0.03

SQL command:
```sql
WITH ExtractedData AS (
    SELECT 
        SUBSTR(
            name,
            INSTR(name, '_deps/boost-build/libs/') + LENGTH('_deps/boost-build/libs/'),
            INSTR(SUBSTR(name, INSTR(name, '_deps/boost-build/libs/') + LENGTH('_deps/boost-build/libs/')), '/') - 1
        ) AS extracted_term,
        dur
    FROM slice
    WHERE name LIKE '_deps/boost-build/libs/%'
),
AggregatedData AS (
    SELECT 
        extracted_term, 
        SUM(dur) AS total_dur
    FROM ExtractedData
    GROUP BY extracted_term
),
TotalDuration AS (
    SELECT SUM(total_dur) AS grand_total
    FROM AggregatedData
)
SELECT 
    extracted_term,
    total_dur,
    ROUND((total_dur * 100.0) / (SELECT grand_total FROM TotalDuration), 2) AS percentage
FROM AggregatedData
ORDER BY percentage DESC;
```

Disabling the tests didn't do much:
log	106573000000	22.18
url	76185000000	15.85
test	68026000000	14.15
serialization	54086000000	11.25
wave	29807000000	6.2
fiber	28517000000	5.93
locale	28087000000	5.84
program_options	20156000000	4.19
filesystem	13865000000	2.88
graph	12836000000	2.67
iostreams	10871000000	2.26
thread	6409000000	1.33
chrono	4603000000	0.96
json	3323000000	0.69
nowide	2912000000	0.61
type_erasure	2360000000	0.49
coroutine	2151000000	0.45
contract	2083000000	0.43
stacktrace	1920000000	0.4
timer	1781000000	0.37
random	1541000000	0.32
container	976000000	0.2
context	886000000	0.18
atomic	320000000	0.07
exception	180000000	0.04
date_time	145000000	0.03

Attempt 2: only include files I need: set(BOOST_INCLUDE_LIBRARIES filesystem system iostreams uuid format algorithm)


filesystem	10911000000	54.16
iostreams	7727000000	38.36
random	1117000000	5.54
atomic	315000000	1.56
exception	75000000	0.37

Now - only the stuff I need is there

total time spent: 38750000000 (cut out 90% of time)