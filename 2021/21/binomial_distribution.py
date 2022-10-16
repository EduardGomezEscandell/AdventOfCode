"""
Generates the function BinomalPair
"""

template = """
#define N_COMBINATIONS {N_COMBINATIONS}

void BinomalPair(size_t i, int * roll, int * weight)
{
    if(i >= N_COMBINATIONS)
    {
        *roll = 0;
        *weight = 0;
        return;
    }

    static const int rolls[{N_COMBINATIONS}] =
    {
        {ROLLS}
    };

    static const int weights[{N_COMBINATIONS}] =
    {
        {WEIGHTS}
    };

    *roll = rolls[i];
    *weight = weights[i];
}
"""

distributions = {}

for i in range(1, 4):
    for j in range(1, 4):
        for k in range(1, 4):
            s = i+j+k
            if s in distributions:
                distributions[s] += 1
            else:
                distributions[s] = 1

rolls = ", ".join([str(d) for d in distributions.keys()])
weights = ", ".join(([str(w) for w in distributions.values()]))
n_universes = str(len(distributions.values()))

result = template.replace("{N_COMBINATIONS}", n_universes)                    \
                 .replace("{ROLLS}", rolls)                                   \
                 .replace("{WEIGHTS}", weights)


print(result)
