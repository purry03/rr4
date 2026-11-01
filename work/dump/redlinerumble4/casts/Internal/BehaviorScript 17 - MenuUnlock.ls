property cam, startTicks, shaunState, mShaun, loopCounter, blinkStartTicks, blinkInterval, changeAnimStartTicks, animState, lookOffset, mBackdrop, oStartButton, theMouseDown, prevMouseDown, prevMouseOver, flashCounter, oTextBox, oAddOn, g_ShaunHandle, mLandrover, gLandrover
global g_Muted, g_NextFrame, g_FadeToBlack, g_NextFramePlease, g_MissionID, g_AddOnList

on beginSprite
  w = sprite(the currentSpriteNum).member
  w.resetWorld()
  shaunState = 0
  loopCounter = 0
  flashCounter = 0
  mShaun = w.model(1)
  w.cloneMotionFromCastmember("ShaunShapeUpMotion", member(13, "Motions").motion[2].name, member(13, "Motions"))
  mShaun.bonesPlayer.queue("ShaunShapeUpMotion", 1)
  mShaun.bonesPlayer.playRate = 1.19999999999999996
  mShaun.bonesPlayer.playNext()
  changeAnimStartTicks = the ticks
  animState = 2
  lookOffset = vector(0, 0, 0)
  mShaun.rotate(0, 180, 0)
  mShaun.worldPosition = vector(40, 30, 0)
  mShaun.transform.scale = vector(0.59999999999999998, 0.59999999999999998, 0.59999999999999998)
  gShaunHandle = w.newGroup("ShaunHandleGroup")
  gShaunHandle.addChild(mShaun)
  blinkStartTicks = the ticks
  blinkInterval = 100
  prevMouseOver = 0
  cam = sprite(1).camera
  cam.fieldOfView = 10
  cam.worldPosition.x = -15
  cam.transform.rotation.x = -5
  tempTexture = w.newTexture("menuBackdropTexture", #fromCastMember, member(51, "Textures"))
  tempTexture.renderFormat = #rgba8888
  nmr = w.newModelResource("MenuBackdropResource", #plane, #both)
  mBackdrop = w.newModel("MenuBackdropModel", nmr)
  mBackdrop.rotate(180, 0, 0)
  mBackdrop.shaderList[1].texture = tempTexture
  mBackdrop.scale(100, 90, 100)
  mBackdrop.worldPosition = vector(-15, 60, -100)
  mBackdrop.shaderList[1].textureTransform.scale(0.69999999999999996, 0.69999999999999996, 0.69999999999999996)
  case g_MissionID of
    3:
      tempTexture = w.newTexture("TextBox1Texture", #fromCastMember, member(105, "Textures"))
      tempTexture1 = w.newTexture("AddOnTexture", #fromCastMember, member(20, "Textures"))
      g_AddOnList[2] = 1
    4:
      tempTexture = w.newTexture("TextBox1Texture", #fromCastMember, member(108, "Textures"))
      tempTexture1 = w.newTexture("AddOnTexture", #fromCastMember, member(23, "Textures"))
      g_AddOnList[5] = 1
    5:
      tempTexture = w.newTexture("TextBox1Texture", #fromCastMember, member(107, "Textures"))
      tempTexture1 = w.newTexture("AddOnTexture", #fromCastMember, member(22, "Textures"))
      g_AddOnList[4] = 1
    9:
      tempTexture = w.newTexture("TextBox1Texture", #fromCastMember, member(106, "Textures"))
      tempTexture1 = w.newTexture("AddOnTexture", #fromCastMember, member(21, "Textures"))
      g_AddOnList[3] = 1
    10:
      tempTexture = w.newTexture("TextBox1Texture", #fromCastMember, member(109, "Textures"))
      tempTexture1 = w.newTexture("AddOnTexture", #fromCastMember, member(24, "Textures"))
      g_AddOnList[6] = 1
  end case
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(80, 340), 0)
  oTextBox = cam.overlay.count
  cam.overlay[oTextBox].blend = 100
  tempTexture1.renderFormat = #rgba8888
  cam.addOverlay(tempTexture1, point(80, 50), 90)
  oAddOn = cam.overlay.count
  cam.overlay[oAddOn].blend = 100
  cam.overlay[oAddOn].scale = 2
  cam.overlay[oAddOn].rotation = 90
  if g_Muted = 0 then
    puppetSound(1, member(34, "Sfx"))
    puppetSound(2, member(32, "Sfx"))
  end if
  startTicks = the ticks
end

on exitFrame
  theMouseDown = the mouseDown
  if cam.overlay[oTextBox].blend < 100 then
    cam.overlay[oTextBox].blend = cam.overlay[oTextBox].blend + 1
  end if
  if cam.overlay[oAddOn].blend < 100 then
    cam.overlay[oAddOn].blend = cam.overlay[oAddOn].blend + 1
  end if
  cam.overlay[oAddOn].rotation = -cam.overlay[oAddOn].rotation * 0.94999999999999996
  mBackdrop.shaderList[1].textureTransform.translate(-0.001, -0.001, 0)
  mShaun.worldPosition.x = mShaun.worldPosition.x * 0.90000000000000002
  prevMouseDown = theMouseDown
  if (the ticks - startTicks) > 500 then
    g_NextFramePlease = 10
    g_FadeToBlack = 1
  end if
end
