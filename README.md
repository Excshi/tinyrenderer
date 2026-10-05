# Run Commands
```bash
// Time
g++ ../tgaimage.cpp ../main.cpp -O3 -Wno-narrowing && time ./a.out

// Profile
g++ ../tgaimage.cpp ../main.cpp -O3 -pg -g -Wno-narrowing && ./a.out && gprof a.out gmon.out > profile.txt
```

## Notes
- Do profiling to see if inlining a function is useful
- You could eliminate divisions by multiplying all the steps in the formula
- If statements can be slow if they're unpredictable (messes with pipelining) so you can try rewriting computations branchlessly
- narrowing and casting floats to ints truncates
- compiler is not likely to precompute float calculations (or move an obviously static value outside a loop to be calced once), so best do it yourself 
- for positive only floats, adding 0.5f before trunc can be way faster than using std::round(), while being functionally equivalent