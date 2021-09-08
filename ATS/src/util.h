template <typename T, int min = 40, int max = 250>
T clamp(T n)
{
    if (n < min)
    {
        return min;
    }

    if (n > max)
    {
        return max;
    }

    return n;
}