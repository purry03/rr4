property w, cam, oFadeToBlack, fadeToBlackBlend, prevFadeToBlackBlend
global g_NextFrame, g_PrevFrame, g_FadeToBlack, g_NextFramePlease

on beginSprite
  g_NextFramePlease = g_NextFrame
  g_FadeToBlack = 0
  fadeToBlackBlend = 100
  w = sprite(the currentSpriteNum).member
  cam = sprite(the currentSpriteNum).camera
  tempTexture = w.newTexture("FadeToBlackTexture", #fromCastMember, member(52, "Textures"))
  cam.addOverlay(tempTexture, point(0, 0), 0)
  oFadeToBlack = cam.overlay.count
  cam.overlay[oFadeToBlack].scale = 100
end

on exitFrame
  cam.overlay[oFadeToBlack].blend = fadeToBlackBlend
  if g_FadeToBlack = 0 then
    if fadeToBlackBlend > 0 then
      fadeToBlackBlend = fadeToBlackBlend - 2
    end if
  else
    if fadeToBlackBlend < 100 then
      fadeToBlackBlend = fadeToBlackBlend + 2
    end if
  end if
  if fadeToBlackBlend = 0 then
    cam.overlay[oFadeToBlack].loc = point(-1000, -1000)
  else
    cam.overlay[oFadeToBlack].loc = point(0, 0)
  end if
end
