property cam, startTicks, shaunState, mShaun, loopCounter, blinkStartTicks, blinkInterval, changeAnimStartTicks, animState, lookOffset, mBackdrop, oStartButton, theMouseDown, prevMouseDown, prevMouseOver, flashCounter
global g_Muted, g_NextFrame, g_FadeToBlack, g_NextFramePlease

on beginSprite
  w = sprite(the currentSpriteNum).member
  w.resetWorld()
  shaunState = 0
  loopCounter = 0
  flashCounter = 0
  mShaun = w.model(1)
  w.cloneMotionFromCastmember("ShaunShapeUpMotion", member(13, "Motions").motion[2].name, member(13, "Motions"))
  w.cloneMotionFromCastmember("ShaunBleatingMotion", member(14, "Motions").motion[2].name, member(14, "Motions"))
  w.cloneMotionFromCastmember("ShaunGrinMotion", member(15, "Motions").motion[2].name, member(15, "Motions"))
  w.cloneMotionFromCastmember("ShaunMunchingMotion", member(16, "Motions").motion[2].name, member(16, "Motions"))
  mShaun.bonesPlayer.blendTime = 800
  mShaun.bonesPlayer.queue("ShaunGrinMotion", 1)
  mShaun.bonesPlayer.playNext()
  changeAnimStartTicks = the ticks
  animState = 2
  lookOffset = vector(0, 0, 0)
  mShaun.rotate(0, 180, 0)
  mShaun.worldPosition = vector(-100, 0, 0)
  blinkStartTicks = the ticks
  blinkInterval = 100
  prevMouseOver = 0
  cam = sprite(1).camera
  cam.fieldOfView = 10
  cam.transform.rotation.x = -5
  tempTexture = w.newTexture("AardmanLogoTexture", #fromCastMember, member(8, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(10, 410), 0)
  tempTexture = w.newTexture("ShaunTheSheepLogoTexture", #fromCastMember, member(9, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 20), 0)
  tempTexture = w.newTexture("StartButtonTexture", #fromCastMember, member(50, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(130, 350), 0)
  oStartButton = cam.overlay.count
  cam.overlay[oStartButton].regPoint = point(64, 32)
  tempTexture = w.newTexture("menuBackdropTexture", #fromCastMember, member(51, "Textures"))
  tempTexture.renderFormat = #rgba8888
  nmr = w.newModelResource("MenuBackdropResource", #plane, #both)
  mBackdrop = w.newModel("MenuBackdropModel", nmr)
  mBackdrop.rotate(180, 0, 0)
  mBackdrop.shaderList[1].texture = tempTexture
  mBackdrop.scale(100, 90, 100)
  mBackdrop.worldPosition = vector(0, 60, -100)
  mBackdrop.shaderList[1].textureTransform.scale(0.69999999999999996, 0.69999999999999996, 0.69999999999999996)
  if g_Muted = 0 then
    puppetSound(1, member(17, "Sfx"))
    puppetSound(2, member(32, "Sfx"))
  end if
  startTicks = the ticks
end

on exitFrame
  if (g_Muted = 0) and (sound(1).member <> member(17, "Sfx")) and (g_NextFramePlease = g_NextFrame) then
    puppetSound(1, member(17, "Sfx"))
  end if
  theMouseDown = the mouseDown
  flashCounter = flashCounter + 1
  if flashCounter > 18 then
    flashCounter = 0
  end if
  if flashCounter <= 10 then
    cam.overlay[oStartButton].blend = 100
  else
    cam.overlay[oStartButton].blend = 50
  end if
  if (the ticks - changeAnimStartTicks) > 500 then
    animState = animState + 1
    changeAnimStartTicks = the ticks
    if animState > 2 then
      animState = 1
    end if
    case animState of
      1:
        mShaun.bonesPlayer.blendTime = 300
        mShaun.bonesPlayer.queue("ShaunMunchingMotion", 1)
        mShaun.bonesPlayer.playNext()
        mShaun.bonesPlayer.currentTime = 400
        lookOffset = vector(0, 100, 120)
        if g_Muted = 0 then
          puppetSound(3, member(18, "Sfx"))
        end if
      2:
        mShaun.bonesPlayer.blendTime = 300
        mShaun.bonesPlayer.queue("ShaunGrinMotion", 1)
        mShaun.bonesPlayer.playNext()
        lookOffset = vector(0, 0, 0)
        puppetSound(3, 0)
    end case
  end if
  tempDiff = the mouseLoc - cam.overlay[oStartButton].loc
  if (abs(tempDiff.locH) < 64) and (abs(tempDiff.locV) < 32) then
    cam.overlay[oStartButton].blend = 100
    if prevMouseOver = 0 then
      if g_Muted = 0 then
        puppetSound(2, member(24, "Sfx"))
      end if
    end if
    prevMouseOver = 1
    if theMouseDown = 1 then
      cam.overlay[oStartButton].scale = 0.90000000000000002
      cam.overlay[oStartButton].regPoint = point(64, 32) - point(5, 5)
    else
      cam.overlay[oStartButton].scale = 1.10000000000000009
      cam.overlay[oStartButton].regPoint = point(64, 32) - point(-5, -5)
    end if
    if (theMouseDown = 0) and (prevMouseDown = 1) then
      g_NextFramePlease = 10
      g_FadeToBlack = 1
      if g_Muted = 0 then
        puppetSound(1, member(25, "Sfx"))
        puppetSound(2, 0)
      end if
    else
    end if
  else
    prevMouseOver = 0
    cam.overlay[oStartButton].scale = 1
    cam.overlay[oStartButton].regPoint = point(64, 32)
  end if
  mBackdrop.shaderList[1].textureTransform.translate(-0.001, -0.001, 0)
  mShaun.worldPosition = mShaun.worldPosition * 0.90000000000000002
  tickDiff = the ticks - blinkStartTicks
  if tickDiff < blinkInterval then
    tempTransform = mShaun.bonesPlayer.bone[22].transform.duplicate()
    tempTransform.position = vector(-12, 4, 0)
    tempTransform.rotation = vector(0, 0, -80)
    mShaun.bonesPlayer.bone[22].transform = tempTransform
  end if
  if (tickDiff >= blinkInterval) or the mouseDown then
    tempTransform = mShaun.bonesPlayer.bone[22].transform.duplicate()
    tempTransform.position = vector(-8, 4, 0)
    tempTransform.rotation = vector(0, 0, -80)
    mShaun.bonesPlayer.bone[22].transform = tempTransform
  end if
  if tickDiff > (blinkInterval + 8) then
    blinkInterval = 100 + random(100)
    blinkStartTicks = the ticks
  end if
  lookVector = vector(0, -(520 - the mouseH), 65 - the mouseV) + lookOffset
  if lookVector.magnitude > 400 then
    lookVector = lookVector.getNormalized() * 400
  end if
  tempTransform = mShaun.bonesPlayer.bone[7].transform.duplicate()
  tempTransform.rotation = lookVector * 0.10000000000000001
  mShaun.bonesPlayer.bone[7].transform = tempTransform
  tempTransform = mShaun.bonesPlayer.bone[20].transform.duplicate()
  tempTransform.rotation = lookVector * 0.05
  mShaun.bonesPlayer.bone[20].transform = tempTransform
  tempTransform = mShaun.bonesPlayer.bone[21].transform.duplicate()
  tempTransform.rotation = lookVector * 0.05
  mShaun.bonesPlayer.bone[21].transform = tempTransform
  prevMouseDown = theMouseDown
end

on mouseDown me
  if g_Muted = 0 then
    puppetSound(2, member(35, "Sfx"))
  end if
end
