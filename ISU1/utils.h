#define MAXWELL_ASSERT_FALSE(expr, error) \
    do {                                  \
        if (expr) throw error(#expr);     \
    } while (0)
