property cam, startTicks, BitzerState, mBitzer, loopCounter, blinkStartTicks, blinkInterval, changeAnimStartTicks, animState, lookOffset, mBackdrop, oStartButton, theMouseDown, prevMouseDown, prevMouseOver, animLength, oTabList, tabPrevMouseOverList, iScoreList, oSubmitButton, oBackButton, flashCounter, backButtonPrevMouseOver, submitButtonPrevMouseOver
global g_Muted, g_NextFrame, g_FadeToBlack, g_NextFramePlease, g_MissionID, g_LevelScoreList, g_LevelPassedList, g_TotalPoints, g_PrevTotalPoints, g_AddOnList, g_SaveGameName

on beginSprite
  w = sprite(the currentSpriteNum).member
  w.resetWorld()
  BitzerState = 0
  loopCounter = 0
  mBitzer = w.model(1)
  w.cloneMotionFromCastmember("BitzerSaluteMotion", member(21, "Motions").motion[2].name, member(21, "Motions"))
  w.cloneMotionFromCastmember("BitzerClipBoardMotion", member(22, "Motions").motion[2].name, member(22, "Motions"))
  w.cloneMotionFromCastmember("BitzerTeaTimeMotion", member(23, "Motions").motion[2].name, member(23, "Motions"))
  changeAnimStartTicks = the ticks
  animLength = 50
  animState = 0
  lookOffset = vector(0, 0, 0)
  mBitzer.rotate(0, 180, 0)
  mBitzer.worldPosition = vector(-100, 0, 0)
  blinkStartTicks = the ticks
  blinkInterval = 100
  prevMouseOver = 0
  backButtonPrevMouseOver = 0
  submitButtonPrevMouseOver = 0
  cam = sprite(1).camera
  cam.fieldOfView = 20
  cam.transform.rotation = vector(-5, 12, 0)
  cam.worldPosition = vector(0, 70, 300)
  iScoreList = []
  repeat with i = 1 to 10
    iScoreList.add(image(256, 32, 32))
    iScoreList[i].useAlpha = 1
    tempString = string(g_LevelScoreList[i])
    repeat while tempString.char.count < 6
      tempString = "0" & tempString
    end repeat
    repeat with j = 1 to 6
      dRect = rect(24 * (j - 1), 0, (24 * (j - 1)) + 24, 32)
      sRect = rect(4, 0, 28, 32)
      tempDigit = integer(tempString.char[j])
      iScoreList[i].copyPixels(member(25 + tempDigit, "Textures").image, dRect, sRect)
    end repeat
    dRect = rect(24 * (7 - 1), 0, 256, 32)
    sRect = rect(0, 0, 1, 1)
    tempDigit = integer(tempString.char[j])
    iScoreList[i].copyPixels(member(25 + tempDigit, "Textures").image, dRect, sRect)
    tempTexture = w.newTexture("Score" & i & "Texture", #fromImageObject, iScoreList[i])
    tempTexture.renderFormat = #rgba8888
    cam.addOverlay(tempTexture, point(300, 14 + (36 * i)), 0)
  end repeat
  iTotal = iScoreList[1].duplicate()
  tempTotal = 0
  repeat with i = 1 to g_LevelScoreList.count
    tempTotal = tempTotal + integer(g_LevelScoreList[i])
  end repeat
  g_TotalPoints = tempTotal
  if g_PrevTotalPoints <> g_TotalPoints then
    tempSaveString = EMPTY
    repeat with i = 1 to 10
      if i = 1 then
        tempSaveString = tempSaveString & g_LevelPassedList[i]
        next repeat
      end if
      tempSaveString = tempSaveString & "," & g_LevelPassedList[i]
    end repeat
    repeat with i = 1 to 6
      tempSaveString = tempSaveString & "," & g_AddOnList[i]
    end repeat
    setPref(g_SaveGameName, tempSaveString)
  end if
  tempString = string(tempTotal)
  repeat while tempString.char.count < 6
    tempString = "0" & tempString
  end repeat
  member("Points", "Textures").text = tempString
  repeat with j = 1 to 6
    dRect = rect(24 * (j - 1), 0, (24 * (j - 1)) + 24, 32)
    sRect = rect(4, 0, 28, 32)
    tempDigit = integer(tempString.char[j])
    iTotal.copyPixels(member(25 + tempDigit, "Textures").image, dRect, sRect)
  end repeat
  tempTexture = w.newTexture("TotalTexture", #fromImageObject, iTotal)
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(300, 430), 0)
  tempTexture = w.newTexture("BitzerTitleTexture", #fromCastMember, member(54, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(30, 10), 0)
  oTabList = []
  tabPrevMouseOverList = []
  repeat with i = 1 to 10
    tempTexture = w.newTexture("tab" & i & "Texture", #fromCastMember, member(54 + i, "Textures"))
    tempTexture.renderFormat = #rgba8888
    cam.addOverlay(tempTexture, point(160, 30 + (36 * i)), 0)
    oTabList.add(cam.overlay.count)
    cam.overlay[oTabList[i]].regPoint = point(128, 16)
    tabPrevMouseOverList.add(0)
    if g_LevelPassedList[i] = 0 then
      cam.overlay[oTabList[i]].blend = 30
    end if
  end repeat
  tempTexture1 = w.newTexture("LPlateTexture", #fromCastMember, member(66, "Textures"))
  tempTexture1.renderFormat = #rgba8880
  tempTexture2 = w.newTexture("PPlateTexture", #fromCastMember, member(67, "Textures"))
  tempTexture2.renderFormat = #rgba8880
  tempTexture3 = w.newTexture("LockedPlateTexture", #fromCastMember, member(68, "Textures"))
  tempTexture3.renderFormat = #rgba8880
  repeat with i = 1 to 10
    case g_LevelPassedList[i] of
      0:
        cam.addOverlay(tempTexture3, point(460, 12 + (36 * i)), 0)
      1:
        cam.addOverlay(tempTexture1, point(460, 12 + (36 * i)), 0)
      2:
        cam.addOverlay(tempTexture2, point(460, 12 + (36 * i)), 0)
    end case
  end repeat
  tempTexture = w.newTexture("BackButtonTexture", #fromCastMember, member(98, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(42, 453), 0)
  oBackButton = cam.overlay.count
  if the runMode contains "Projector" then
    tempTexture = w.newTexture("GoToShockwaveTexture", #fromCastMember, member(154, "Textures"))
    tempTexture.renderFormat = #rgba8888
    cam.addOverlay(tempTexture, point(454, 412), 0)
    tabPrevMouseOverList.add(0)
  else
    tempTexture = w.newTexture("SubmitButtonTexture", #fromCastMember, member(65, "Textures"))
    tempTexture.renderFormat = #rgba8888
    cam.addOverlay(tempTexture, point(207, 443), 0)
    oSubmitButton = cam.overlay.count
    cam.overlay[oSubmitButton].regPoint = point(100, 16)
    cam.overlay[oSubmitButton].blend = 40
    tabPrevMouseOverList.add(0)
  end if
  tempTexture = w.newTexture("menuBackdropTexture", #fromCastMember, member(53, "Textures"))
  tempTexture.renderFormat = #rgba8888
  nmr = w.newModelResource("MenuBackdropResource", #plane, #both)
  mBackdrop = w.newModel("MenuBackdropModel", nmr)
  mBackdrop.rotate(180, 0, 0)
  mBackdrop.shaderList[1].texture = tempTexture
  mBackdrop.scale(300, 300, 300)
  mBackdrop.worldPosition = vector(-100, 60, -100)
  mBackdrop.shaderList[1].textureTransform.scale(0.20000000000000001, 0.20000000000000001, 0.20000000000000001)
  if g_Muted = 0 then
    puppetSound(1, member(17, "Sfx"))
  end if
  startTicks = the ticks
  sprite(the currentSpriteNum).loc = point(400, 240)
end

on exitFrame
  if (g_Muted = 0) and (sound(1).member <> member(17, "Sfx")) and (g_NextFramePlease = g_NextFrame) then
    puppetSound(1, member(17, "Sfx"))
  end if
  theMouseDown = the mouseDown
  repeat with i = 1 to oTabList.count
    if g_LevelPassedList[i] <> 0 then
      tempDiff = the mouseLoc - cam.overlay[oTabList[i]].loc
      if (abs(tempDiff.locH) < 128) and (abs(tempDiff.locV) < 18) then
        if tabPrevMouseOverList[i] = 0 then
          if g_Muted = 0 then
            puppetSound(2, member(24, "Sfx"))
          end if
        end if
        tabPrevMouseOverList[i] = 1
        if theMouseDown = 1 then
          cam.overlay[oTabList[i]].scale = 0.90000000000000002
          cam.overlay[oTabList[i]].regPoint = point(128, 16) - point(14, 2)
        else
          cam.overlay[oTabList[i]].scale = 1.10000000000000009
          cam.overlay[oTabList[i]].regPoint = point(128, 16) - point(-14, -2)
        end if
        if (theMouseDown = 0) and (prevMouseDown = 1) then
          g_NextFramePlease = 15
          g_MissionID = i
          g_FadeToBlack = 1
          if g_Muted = 0 then
            puppetSound(1, member(25, "Sfx"))
            puppetSound(2, 0)
            puppetSound(3, 0)
          end if
        end if
        next repeat
      end if
      tabPrevMouseOverList[i] = 0
      cam.overlay[oTabList[i]].scale = 1
      cam.overlay[oTabList[i]].regPoint = point(128, 16)
    end if
  end repeat
  tempDiff = the mouseLoc - cam.overlay[oBackButton].loc
  if (abs(tempDiff.locH) < 32) and (abs(tempDiff.locV) < 16) then
    if backButtonPrevMouseOver = 0 then
      if g_Muted = 0 then
        puppetSound(2, member(24, "Sfx"))
      end if
    end if
    backButtonPrevMouseOver = 1
    if theMouseDown = 1 then
      cam.overlay[oBackButton].scale = 0.90000000000000002
      cam.overlay[oBackButton].regPoint = point(32, 8) - point(3, 2)
    else
      cam.overlay[oBackButton].scale = 1.10000000000000009
      cam.overlay[oBackButton].regPoint = point(32, 8) - point(-3, -2)
    end if
    if (theMouseDown = 0) and (prevMouseDown = 1) then
      g_NextFramePlease = 5
      g_FadeToBlack = 1
      startButtonFlashing = 0
      if g_Muted = 0 then
        puppetSound(1, member(25, "Sfx"))
        puppetSound(2, 0)
        puppetSound(3, 0)
      end if
    else
    end if
  else
    backButtonPrevMouseOver = 0
    cam.overlay[oBackButton].scale = 1
    cam.overlay[oBackButton].regPoint = point(32, 8)
  end if
  if not (the runMode) contains "Projector" then
    tempDiff = the mouseLoc - cam.overlay[oSubmitButton].loc
    if (abs(tempDiff.locH) < 100) and (abs(tempDiff.locV) < 18) and (g_TotalPoints > 0) and (g_TotalPoints <> g_PrevTotalPoints) then
      if submitButtonPrevMouseOver = 0 then
        if g_Muted = 0 then
          puppetSound(2, member(24, "Sfx"))
        end if
      end if
      submitButtonPrevMouseOver = 1
      cam.overlay[oSubmitButton].blend = 100
      if theMouseDown = 1 then
        cam.overlay[oSubmitButton].scale = 0.90000000000000002
        cam.overlay[oSubmitButton].regPoint = point(100, 16) - point(14, 2)
      else
        cam.overlay[oSubmitButton].scale = 1.10000000000000009
        cam.overlay[oSubmitButton].regPoint = point(100, 16) - point(-14, -2)
      end if
      if (theMouseDown = 0) and (prevMouseDown = 1) then
        put "SUBMIT CALLED!"
        sendSprite(the currentSpriteNum, #InitList, "shaunthesheep", "Lamb Rover 4x4")
        sendSprite(the currentSpriteNum, #branchToScores)
        g_PrevFrame = 10
        if g_Muted = 0 then
          puppetSound(1, member(25, "Sfx"))
          puppetSound(2, 0)
          puppetSound(3, 0)
        end if
      end if
    else
      submitButtonPrevMouseOver = 0
      cam.overlay[oSubmitButton].scale = 1
      cam.overlay[oSubmitButton].regPoint = point(100, 16)
      if g_PrevTotalPoints < g_TotalPoints then
        flashCounter = flashCounter + 1
        if flashCounter > 10 then
          flashCounter = 0
        end if
        if flashCounter <= 6 then
          cam.overlay[oSubmitButton].blend = 100
        else
          cam.overlay[oSubmitButton].blend = 40
        end if
      end if
    end if
  end if
  mBackdrop.shaderList[1].textureTransform.translate(-0.001, -0.001, 0)
  mBitzer.worldPosition = mBitzer.worldPosition * 0.90000000000000002
  if (the ticks - changeAnimStartTicks) > animLength then
    animState = animState + 1
    changeAnimStartTicks = the ticks
    if animState > 3 then
      animState = 1
    end if
    case animState of
      1:
        mBitzer.bonesPlayer.blendTime = 200
        mBitzer.bonesPlayer.queue("BitzerSaluteMotion", 1)
        mBitzer.bonesPlayer.playNext()
        if g_Muted = 0 then
          puppetSound(3, member(26, "Sfx"))
        end if
        animLength = 100
      2:
        mBitzer.bonesPlayer.blendTime = 300
        mBitzer.bonesPlayer.queue("BitzerClipBoardMotion", 1)
        mBitzer.bonesPlayer.playNext()
        if g_Muted = 0 then
          puppetSound(3, member(27, "Sfx"))
        end if
        animLength = 400
      3:
        mBitzer.bonesPlayer.blendTime = 300
        mBitzer.bonesPlayer.queue("BitzerTeaTimeMotion", 1)
        mBitzer.bonesPlayer.playNext()
        lookOffset = vector(0, 0, 0)
        if g_Muted = 0 then
          puppetSound(3, member(28, "Sfx"))
        end if
        animLength = 200
    end case
  end if
  tickDiff = the ticks - blinkStartTicks
  if tickDiff < blinkInterval then
    tempTransform = mBitzer.bonesPlayer.bone[14].transform.duplicate()
    tempTransform.position = vector(-12, 4, 0)
    tempTransform.rotation = vector(0, 0, -80)
    mBitzer.bonesPlayer.bone[14].transform = tempTransform
  end if
  if (tickDiff >= blinkInterval) or the mouseDown then
    tempTransform = mBitzer.bonesPlayer.bone[15].transform.duplicate()
    tempTransform.position = vector(-8, 4, 0)
    tempTransform.rotation = vector(0, 0, -80)
    mBitzer.bonesPlayer.bone[15].transform = tempTransform
  end if
  if tickDiff > (blinkInterval + 8) then
    blinkInterval = 100 + random(100)
    blinkStartTicks = the ticks
  end if
  prevMouseDown = theMouseDown
end

on mouseDown me
end
