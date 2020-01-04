#ifndef _alg_arrays
#define _alg_arrays

NAMESPACE_BEGIN(alg)

//////////////////////////////////////////////////////////////////////////
// binary search

template< typename T, typename compare_less >
int array_binary_search(T a[], int low, int high, T target)
{
  while (low <= high)
  {
    int middle = low + (high - low)/2;
    if (compare_less(target, a[middle]))
      high = middle - 1;
    else if (compare_less(a[middle], target))
      low = middle + 1;
    else
      return middle;
  }
  return -1;
}

template< typename T, typename compare_less >
int array_binary_search_cmp(T a[], int low, int high, T target)
{
  while (low <= high)
  {
    int middle = low + (high - low)/2;
    if (target < a[middle])
      high = middle - 1;
    else if (a[middle] < target)
      low = middle + 1;
    else
      return middle;
  }
  return -1;
}

template< typename T,typename K>
int array_binary_search_key_cmp(T a[], int low, int high, K target)
{
  while (low <= high)
  {
    int middle = low + (high - low)/2;
    if (target < a[middle])
      high = middle - 1;
    else if (a[middle] < target)
      low = middle + 1;
    else
      return middle;
  }
  return -1;
}

template< typename T, typename IterT, typename compare_less >
IterT generic_binary_search(IterT begin, IterT end, T target)
{
  IterT initial_end = end;
  while (begin < end)
  {
    IterT middle = begin + (end - begin - 1)/2;
    if (compare_less(target, *middle))
      end = middle;
    else if (compare_less(*middle, target))
      begin = middle + 1;
    else
      return middle;
  }
  return initial_end;
}

template< typename T, typename IterT >
IterT generic_binary_search_cmp(IterT begin, IterT end, T target)
{
  IterT initial_end = end;
  while (begin < end)
  {
    IterT middle = begin + (end - begin - 1)/2;
    if (target < *middle)
      end = middle;
    else if (*middle < target)
      begin = middle + 1;
    else
      return middle;
  }
  return initial_end;
}

template< typename T >
int array_linear_search_cmp(const T a[], int low, int high, T target)
{
  for (int i = low; i != high; ++i)
    if (target == a[i])
      return i;
  return -1;
}

template< typename T, typename IterT, typename compare >
IterT generic_linear_search(IterT begin, IterT end, T target)
{
  for (IterT i = begin; i != end; ++i)
    if (compare(target, *i))
      return i;
  return end;
}

// reverse
template<class T>
void reverse (T* a, int n)
{
  int i, j;
  j = n / 2;
  --n;
  for (i=0; i < j; i++)
  {
    T tmp = a[i];
    a[i] = a[n-i];
    a[n-i] = tmp;
  }
}

// rotate
template<class T>
void rotate (T a[], int n, int k)
{
  if (a == NULL || n <= 0)
    return;
  if (k < 0 || k >= n)
  {
    k %= n;
    if (k < 0)
      k += n;
  }
  if (k == 0)
    return;

  reverse (a, k);
  reverse (a + k, n - k);
  reverse (a, n);
}

NAMESPACE_END(alg)
#endif // _alg_arrays