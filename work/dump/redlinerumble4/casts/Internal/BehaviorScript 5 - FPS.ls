property fps, startTicks, frameCounter, FPSReadOutTexture
global g_FPSFactor, g_FPSFactorDec, g_FPSFactorFrac

on beginSprite
  g_FPSFactor = 1.0
  startTicks = the ticks
  frameCounter = 0
end

on enterFrame
  frameCounter = frameCounter + 1
  if frameCounter = 60 then
    g_FPSFactor = (the ticks - startTicks) / 60.0
    if g_FPSFactor > 4 then
      g_FPSFactor = 4
    end if
    tempResult = string(60 / g_FPSFactor)
    g_FPSFactorDec = integer(g_FPSFactor - 0.5)
    g_FPSFactorFrac = g_FPSFactor - g_FPSFactorDec
    frameCounter = 0
    startTicks = the ticks
  end if
end
