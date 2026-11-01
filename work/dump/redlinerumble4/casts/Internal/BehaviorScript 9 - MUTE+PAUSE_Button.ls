property prevMouseDown, tMute, cam, oPaused, oBranding, oBackground, buttonRect
global g_Muted, g_Paused, g_NextFrame

on beginSprite
  w = sprite(the currentSpriteNum).member
  cam = sprite(1).camera
  tMute = w.newTexture("MuteTexture", #fromCastMember, member(1, "Textures"))
  tMute.renderFormat = #rgba8888
  sprite(1).camera.addOverlay(tMute, point(776, 460), 0)
  tempTexture = w.newTexture("PausedBackground", #fromCastMember, member("PauseBackground", "Textures"))
  tempTexture.renderFormat = #rgba8888
  sprite(1).camera.addOverlay(tempTexture, point(0, 0), 0)
  oBackground = sprite(1).camera.overlay.count
  sprite(1).camera.overlay[oBackground].scale = 12.5
  sprite(1).camera.overlay[oBackground].blend = 0
  tempTexture = w.newTexture("PausedTexture", #fromCastMember, member(3, "Textures"))
  tempTexture.renderFormat = #rgba8888
  sprite(1).camera.addOverlay(tempTexture, point(336, 100), 0)
  oPaused = sprite(1).camera.overlay.count
  sprite(1).camera.overlay[oPaused].blend = 0
  tempTexture = w.newTexture("PauseBranding", #fromCastMember, member("PauseBrandingBtn_Texture", "Textures"))
  tempTexture.renderFormat = #rgba8888
  sprite(1).camera.addOverlay(tempTexture, point(282, 430), 0)
  oBranding = sprite(1).camera.overlay.count
  sprite(1).camera.overlay[oBranding].blend = 0
  buttonRect = rect(282, 450, 538, 480)
  if g_Muted = 1 then
    tMute.member = member(2, "Textures")
  end if
end

on exitFrame
  if g_Paused = 1 then
    if sprite(1).camera.overlay[oPaused].blend < 100 then
      sprite(1).camera.overlay[oPaused].blend = sprite(1).camera.overlay[oPaused].blend + 5
      sprite(1).camera.overlay[oBranding].blend = sprite(1).camera.overlay[oBranding].blend + 5
      sprite(1).camera.overlay[oBackground].blend = sprite(1).camera.overlay[oBackground].blend + 5
    end if
    mousePosition = _mouse.mouseLoc
    if mousePosition.inside(buttonRect) then
      _player.cursor(280)
      if _mouse.mouseDown and (prevMouseDown = 0) then
        gotoNetPage("http://www.addictinggames.com", "_new")
      end if
    else
      _player.cursor(-1)
    end if
  else
    if sprite(1).camera.overlay[oPaused].blend > 0 then
      sprite(1).camera.overlay[oPaused].blend = sprite(1).camera.overlay[oPaused].blend - 5
      sprite(1).camera.overlay[oBranding].blend = sprite(1).camera.overlay[oBranding].blend - 5
      sprite(1).camera.overlay[oBackground].blend = sprite(1).camera.overlay[oBackground].blend - 5
    end if
  end if
  if the mouseDown and (prevMouseDown = 0) then
    tempDiff = the mouseLoc - point(776, 460)
    if (abs(tempDiff.locH) < 16) and (abs(tempDiff.locV) < 16) then
      put "ButtonMuteHit"
      if g_Muted = 1 then
        sound(5).play()
        g_Muted = 0
        tMute.member = member(1, "Textures")
      else
        g_Muted = 1
        puppetSound(1, 0)
        puppetSound(2, 0)
        puppetSound(3, 0)
        puppetSound(4, 0)
        sound(5).pause()
        puppetSound(6, 0)
        tMute.member = member(2, "Textures")
      end if
    end if
  end if
  prevMouseDown = the mouseDown
end

on keyDown
  if (the key = "p") and (g_NextFrame > 19) then
    if g_Paused = 0 then
      g_Paused = 1
      if g_Muted = 0 then
        sound(1).pause()
        sound(2).pause()
        sound(3).pause()
        sound(4).pause()
        sound(5).pause()
        sound(6).pause()
      end if
    else
      g_Paused = 0
      if g_Muted = 0 then
        sound(1).play()
        sound(2).play()
        sound(3).play()
        sound(4).play()
        sound(5).play()
        sound(6).play()
      end if
    end if
  end if
end
