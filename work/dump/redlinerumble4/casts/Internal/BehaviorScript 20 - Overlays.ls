property w, cam, oRedBar, oSkull, oRedSkull, oGameOver, prevDamagePercent, skullBlend, displayDamagePercent, oLogo, oYellowBar, prevBoostPercent, displayBoostPercent, oLevelSmashed, gameOverTexture, prevGameOverID, oPickups, pickupsTexture, pickupsStartTicks
global g_Paused, g_DamagePercent, g_BoostPercent, g_PickupOverlay, g_GameOverID

on beginSprite
  w = sprite(the currentSpriteNum).member
  cam = sprite(the currentSpriteNum).camera
  prevDamagePercent = 0
  prevBoostPercent = 0
  prevGameOverID = 0
  g_PickupOverlay = 0
  flashTicks = the ticks
  displayDamagePercent = g_DamagePercent
  displayBoostPercent = g_BoostPercent
  tempTexture = w.newTexture("DamageBarTexture", #fromCastMember, member(12, "Textures"))
  tempTexture.renderFormat = #rgba8880
  cam.addOverlay(tempTexture, point(0, 8), 0)
  tempTexture = w.newTexture("DamageRedBarTexture", #fromCastMember, member(13, "Textures"))
  tempTexture.renderFormat = #rgba8880
  cam.addOverlay(tempTexture, point(-256, 8), 0)
  oRedBar = cam.overlay.count
  tempTexture = w.newTexture("DamageSkullTexture", #fromCastMember, member(14, "Textures"))
  tempTexture.renderFormat = #rgba4444
  cam.addOverlay(tempTexture, point(214, 0), 0)
  oSkull = cam.overlay.count
  tempTexture = w.newTexture("DamageRedSkullTexture", #fromCastMember, member(15, "Textures"))
  tempTexture.renderFormat = #rgba4444
  cam.addOverlay(tempTexture, point(246, 1), 0)
  oRedSkull = cam.overlay.count
  tempTexture = w.newTexture("BoostBarTexture", #fromCastMember, member(16, "Textures"))
  tempTexture.renderFormat = #rgba8880
  cam.addOverlay(tempTexture, point(544, 8), 0)
  tempTexture = w.newTexture("BoostYellowBarTexture", #fromCastMember, member(17, "Textures"))
  tempTexture.renderFormat = #rgba8880
  cam.addOverlay(tempTexture, point(800, 8), 0)
  oYellowBar = cam.overlay.count
  tempTexture = w.newTexture("BoostLogoTexture", #fromCastMember, member(18, "Textures"))
  tempTexture.renderFormat = #rgba4444
  cam.addOverlay(tempTexture, point(505, 2), 0)
  oLogo = cam.overlay.count
end

on exitFrame
  if g_Paused = 0 then
    if (prevGameOverID = 0) and (g_GameOverID > 0) then
      case g_GameOverID of
        1:
          gameOverTexture.member = member(94, "Textures")
        2:
          gameOverTexture.member = member(95, "Textures")
        3:
          gameOverTexture.member = member(96, "Textures")
        4:
          gameOverTexture.member = member(97, "Textures")
      end case
    end if
    if g_PickupOverlay <> 0 then
      case g_PickupOverlay of
        1:
          pickupsTexture.member = member(56, "Textures")
        2:
          pickupsTexture.member = member(57, "Textures")
        3:
          pickupsTexture.member = member(58, "Textures")
        4:
          pickupsTexture.member = member(59, "Textures")
        5:
          pickupsTexture.member = member(60, "Textures")
        6:
          pickupsTexture.member = member(61, "Textures")
      end case
      cam.overlay[oPickups].blend = 100
      g_PickupOverlay = 0
      pickupsStartTicks = the ticks
    end if
    if g_DamagePercent > 1 then
      g_DamagePercent = 1
    end if
    if g_GameOverID <> 0 then
      cam.overlay[oGameOver].loc = point(272, 100)
      cam.overlay[oGameOver].rotation = cam.overlay[oGameOver].rotation * -0.80000000000000004
    end if
    displayDamagePercent = displayDamagePercent + ((g_DamagePercent - displayDamagePercent) * 0.20000000000000001)
    if prevDamagePercent <> g_DamagePercent then
      skullBlend = 100
    end if
    cam.overlay[oRedBar].loc = point(-254 + (displayDamagePercent * 254), 8)
    cam.overlay[oRedSkull].blend = skullBlend
    if (skullBlend > 0) and (g_DamagePercent < 1) then
      skullBlend = skullBlend - 5
    end if
    prevDamagePercent = g_DamagePercent
    displayBoostPercent = displayBoostPercent + ((g_BoostPercent - displayBoostPercent) * 0.20000000000000001)
    cam.overlay[oYellowBar].loc = point(800 - (displayBoostPercent * 250), 8)
    prevBoostPercent = g_DamagePercent
    prevGameOverID = g_GameOverID
  end if
end
