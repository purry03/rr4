global g_NextFrame

on exitFrame me
  g_NextFrame = g_NextFrame + 1
  go(g_NextFrame)
end
