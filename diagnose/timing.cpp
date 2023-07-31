namespace timing {
std::chrono::high_resolution_clock::time_point global_tp;

static void start()
{
  global_tp = std::chrono::high_resolution_clock::now();
}

static auto duration()
{
  const auto dur = std::chrono::high_resolution_clock::now() - global_tp;
  return std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
}
