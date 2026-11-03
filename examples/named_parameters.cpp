class Worker {
public:
    int bias;
    [[gnu::noinline]] int process(int count);
};

int Worker::process(int count) { return count + bias; }

[[gnu::noinline]] int overloaded(int count) { return count + 1; }
[[gnu::noinline]] long long overloaded(long long count) { return count + 2; }

struct LargeResult { long long a, b, c; };
[[gnu::noinline]] LargeResult make_large(int count) { return {count, 2, 3}; }

[[gnu::always_inline]] static inline int Inlined(int secret) { return secret * 3; }
int inline_host(int value) { return Inlined(value) + 1; }
