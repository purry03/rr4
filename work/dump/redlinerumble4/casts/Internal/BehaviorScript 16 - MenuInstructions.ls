property cam, startTicks, MotherState, mMother, loopCounter, blinkStartTicks, blinkInterval, changeAnimStartTicks, animState, lookOffset, mBackdrop, oStartButton, theMouseDown, prevMouseDown, prevMouseOver, animLength, oTabList, tabPrevMouseOverList, iScoreList, oSubmitButton, mItemList, oTextBox, tTextList, itemID, gItems, keyStartTicks, keyID, mCursorKey1, mCursorKey2, mCursorKey3, mCursorKey4, startButtonFlashing, flashCounter, oBackButton, backButtonPrevMouseOver, startButtonPrevMouseOver, mBFS, mNOSCan, mSpaceBarKey, mStopBlock, mCone, mArrow, mStopBlockArrow
global g_Muted, g_NextFrame, g_FadeToBlack, g_NextFramePlease, g_MissionID, g_LevelScoreList, g_LevelPassedList

on beginSprite
  w = sprite(the currentSpriteNum).member
  w.resetWorld()
  member(1, "w3D").resetWorld()
  member(10, "w3D").resetWorld()
  MotherState = 0
  loopCounter = 0
  mItemList = []
  tTextList = []
  itemID = 1
  keyStartTicks = the ticks
  keyID = 1
  startButtonFlashing = 0
  flashCounter = 0
  startButtonPrevMouseOver = 0
  backButtonPrevMouseOver = 0
  w.deleteModel(1)
  mMother = w.cloneModelFromCastmember("MotherClone", member(6, "w3D").model(1).name, member(6, "w3D"))
  mMother.rotate(0, 160, 0)
  mMother.worldPosition = vector(-100, 10, 0)
  w.cloneMotionFromCastmember("MotherNodMotion", member(24, "Motions").motion[2].name, member(24, "Motions"))
  w.cloneMotionFromCastmember("MotherSmileMotion", member(25, "Motions").motion[2].name, member(25, "Motions"))
  w.cloneMotionFromCastmember("MotherFearMotion", member(8, "Motions").motion[2].name, member(8, "Motions"))
  mMother.bonesPlayer.blendTime = 200
  mMother.bonesPlayer.queue("MotherNodMotion", 1)
  mMother.bonesPlayer.playNext()
  gItems = w.newGroup("ItemsGroup")
  case g_MissionID of
    1:
      gKeys = w.newGroup("KeysGroup")
      gKeysHandle = w.newGroup("KeysHandleGroup")
      gKeysHandle.addChild(gKeys)
      mItemList.add(gKeysHandle)
      mCursorKey1 = w.cloneModelFromCastmember("CursorKeyClone1", "CursorKey_000", member(10, "w3D"))
      mCursorKey2 = mCursorKey1.clone("CursorKeyClone2")
      mCursorKey3 = mCursorKey1.clone("CursorKeyClone3")
      mCursorKey4 = mCursorKey1.clone("CursorKeyClone4")
      gKeys.addChild(mCursorKey1)
      gKeys.addChild(mCursorKey2)
      gKeys.addChild(mCursorKey3)
      gKeys.addChild(mCursorKey4)
      mCursorKey2.translate(30, 0, 0)
      mCursorKey2.transform.scale = vector(-1, 1, 1)
      mCursorKey3.translate(15, 0, 0)
      mCursorKey3.rotate(0, -90, 0)
      mCursorKey3.transform.scale = vector(1, 1, -1)
      mCursorKey4.translate(15, 0, 15)
      mCursorKey4.rotate(0, 90, 0)
      gKeys.rotate(-30, 180, 0)
      gKeys.transform.position = vector(-90, 30, 0)
      gItems.addChild(gKeysHandle)
      mStopBlock = w.cloneModelFromCastmember("StopBlockClone", "StopBlock_000", member(1, "w3D"))
      mStopBlockArrow = w.cloneModelFromCastmember("StopBlockArrowClone", "ArrowStopBlock_000", member(1, "w3D"))
      mStopBlock.addChild(mStopBlockArrow)
      mStopBlock.scale(0.13, 0.13, 0.13)
      mStopBlock.transform.position = vector(-100, 30, 0)
      gStopBlock = w.newGroup("StopBlockGroup")
      gStopBlock.addChild(mStopBlock)
      mItemList.add(gStopBlock)
      gItems.addChild(gStopBlock)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(72, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(73, "Textures")))
      tTextList[2].renderFormat = #rgba8888
    2:
      mCone = w.cloneModelFromCastmember("ConeClone", "TrafficCone_000", member(1, "w3D"))
      mConeArrow = w.cloneModelFromCastmember("ArrowClone", "ArrowAroundConeClockWise_000", member(1, "w3D"))
      mCone.addChild(mConeArrow)
      mCone.scale(0.44, 0.44, 0.44)
      mConeArrow.scale(0.59999999999999998, 0.59999999999999998, 0.59999999999999998)
      mCone.rotate(30, 0, 0)
      mConeArrow.rotate(0, -120, 0)
      mCone.transform.position = vector(-100, 40, 0)
      gCone = w.newGroup("ConeGroup")
      gCone.addChild(mCone)
      mItemList.add(gCone)
      gItems.addChild(gCone)
      mConeCrushed = w.cloneModelFromCastmember("ConeCrushedClone", "TrafficConeCrushed_000", member(1, "w3D"))
      mConeCrushedArrow = w.cloneModelFromCastmember("ArrowCrushedClone", "ArrowAroundConeClockWise_000", member(1, "w3D"))
      mConeCrushed.addChild(mConeCrushedArrow)
      mConeCrushed.scale(0.44, 0.44, 0.44)
      mConeCrushedArrow.scale(0.59999999999999998, 0.59999999999999998, 0.59999999999999998)
      mConeCrushed.rotate(30, 0, 0)
      mConeCrushedArrow.rotate(0, -120, 0)
      mConeCrushed.transform.position = vector(-100, 40, 0)
      gConeCrushed = w.newGroup("ConeCrushedGroup")
      gConeCrushed.addChild(mConeCrushed)
      mItemList.add(gConeCrushed)
      gItems.addChild(gConeCrushed)
      mStopBlock = w.cloneModelFromCastmember("StopBlockClone", "StopBlock_000", member(1, "w3D"))
      mStopBlockArrow = w.cloneModelFromCastmember("StopBlockArrowClone", "ArrowStopBlock_000", member(1, "w3D"))
      mStopBlock.addChild(mStopBlockArrow)
      mStopBlock.scale(0.13, 0.13, 0.13)
      mStopBlock.transform.position = vector(-100, 30, 0)
      gStopBlock = w.newGroup("StopBlockGroup")
      gStopBlock.addChild(mStopBlock)
      mItemList.add(gStopBlock)
      gItems.addChild(gStopBlock)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(74, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(77, "Textures")))
      tTextList[2].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox3Texture", #fromCastMember, member(73, "Textures")))
      tTextList[3].renderFormat = #rgba8888
    3:
      mBale = w.cloneModelFromCastmember("BaleClone", "StrawBale_000", member(1, "w3D"))
      mBaleArrow = w.cloneModelFromCastmember("BaleArrowClone", "ArrowBaleBarge_000", member(1, "w3D"))
      mBale.addChild(mBaleArrow)
      mBale.scale(0.14000000000000001, 0.14000000000000001, 0.14000000000000001)
      mBale.transform.position = vector(-100, 30, 0)
      mBale.rotate(20, 0, 0)
      gBale = w.newGroup("BaleGroup")
      gBale.addChild(mBale)
      mItemList.add(gBale)
      gItems.addChild(gBale)
      mCone = w.cloneModelFromCastmember("ConeClone", "TrafficCone_000", member(1, "w3D"))
      mConeArrow = w.cloneModelFromCastmember("ArrowClone", "ArrowAroundConeClockWise_000", member(1, "w3D"))
      mCone.addChild(mConeArrow)
      mCone.scale(0.44, 0.44, 0.44)
      mConeArrow.scale(0.59999999999999998, 0.59999999999999998, 0.59999999999999998)
      mCone.rotate(30, 0, 0)
      mConeArrow.rotate(0, -120, 0)
      mCone.transform.position = vector(-100, 40, 0)
      gCone = w.newGroup("ConeGroup")
      gCone.addChild(mCone)
      mItemList.add(gCone)
      gItems.addChild(gCone)
      mStopBlock = w.cloneModelFromCastmember("StopBlockClone", "StopBlock_000", member(1, "w3D"))
      mStopBlockArrow = w.cloneModelFromCastmember("StopBlockArrowClone", "ArrowStopBlock_000", member(1, "w3D"))
      mStopBlock.addChild(mStopBlockArrow)
      mStopBlock.scale(0.13, 0.13, 0.13)
      mStopBlock.transform.position = vector(-100, 30, 0)
      gStopBlock = w.newGroup("StopBlockGroup")
      gStopBlock.addChild(mStopBlock)
      mItemList.add(gStopBlock)
      gItems.addChild(gStopBlock)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(75, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(74, "Textures")))
      tTextList[2].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox3Texture", #fromCastMember, member(73, "Textures")))
      tTextList[3].renderFormat = #rgba8888
    4:
      gKeys = w.newGroup("KeysGroup")
      gKeysHandle = w.newGroup("KeysHandleGroup")
      gKeysHandle.addChild(gKeys)
      mItemList.add(gKeysHandle)
      mCursorKey1 = w.cloneModelFromCastmember("CursorKeyClone1", "CursorKey_000", member(10, "w3D"))
      mCursorKey2 = mCursorKey1.clone("CursorKeyClone2")
      mCursorKey3 = mCursorKey1.clone("CursorKeyClone3")
      mCursorKey4 = mCursorKey1.clone("CursorKeyClone4")
      mSpaceBarKey = w.cloneModelFromCastmember("SpaceBarKeyClone", "SpaceBarKey_000", member(10, "w3D"))
      mNOSCan = w.cloneModelFromCastmember("NOSCanClone", "NOSCan_001", member(1, "w3D"))
      mNOSCan.scale(1.5, 1.5, 1.5)
      mNOSCan.rotate(-30, 180, 0)
      mNOSCan.translate(-100, -10, -60)
      gKeys.addChild(mCursorKey1)
      gKeys.addChild(mCursorKey2)
      gKeys.addChild(mCursorKey3)
      gKeys.addChild(mCursorKey4)
      gKeys.addChild(mSpaceBarKey)
      gKeys.addChild(mNOSCan)
      mCursorKey2.translate(30, 0, 0)
      mCursorKey2.transform.scale = vector(-1, 1, 1)
      mCursorKey3.translate(15, 0, 0)
      mCursorKey3.rotate(0, -90, 0)
      mCursorKey3.transform.scale = vector(1, 1, -1)
      mCursorKey4.translate(15, 0, 15)
      mCursorKey4.rotate(0, 90, 0)
      mSpaceBarKey.translate(100, 0, 0)
      gKeys.rotate(-30, 180, 0)
      gKeys.transform.position = vector(-50, 24, 0)
      gKeys.scale(0.5, 0.5, 0.5)
      gItems.addChild(gKeysHandle)
      mCheckPoint = w.cloneModelFromCastmember("CheckPointClone", "CheckPoint_000", member(1, "w3D"))
      mCheckPointArrow = w.cloneModelFromCastmember("CheckPointArrowClone", "ArrowCheckPoint_000", member(1, "w3D"))
      mCheckPointArrow.shaderList[3].texture.renderFormat = #rgba8888
      mCheckPoint.addChild(mCheckPointArrow)
      mCheckPoint.scale(0.08, 0.08, 0.08)
      mCheckPoint.rotate(5, 0, 0)
      mCheckPointArrow.transform.rotation = vector(0, -120, 0)
      mCheckPoint.transform.position = vector(-100, 30, 0)
      gCheckPoint = w.newGroup("CheckPointGroup")
      gCheckPoint.addChild(mCheckPoint)
      mItemList.add(gCheckPoint)
      gItems.addChild(gCheckPoint)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(87, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(76, "Textures")))
      tTextList[2].renderFormat = #rgba8888
    5:
      mBarn = w.cloneModelFromCastmember("BarnObjectsClone", "TILE_BarnObjects_004", member(1, "w3D"))
      mSnorkel = w.cloneModelFromCastmember("SnorkelClone", "LandroverSnorkel_000", member(1, "w3D"))
      mBarn.addChild(mSnorkel)
      mSnorkel.transform.position = vector(10, -280, -50)
      mSnorkel.scale(7, 7, 7)
      mBarn.scale(0.10000000000000001, 0.10000000000000001, 0.10000000000000001)
      mBarn.rotate(0, 180, 0)
      mBarn.shaderList[9].blend = 0
      mBarn.transform.position = vector(-80, 10, 0)
      gBarn = w.newGroup("BarnGroup")
      gBarn.addChild(mBarn)
      mItemList.add(gBarn)
      gItems.addChild(gBarn)
      mAppleTree = w.cloneModelFromCastmember("AppleTreeClone", "AppleTree_000", member(1, "w3D"))
      mAppleTree.shaderList[2].texture.renderFormat = #rgba8888
      mAppleTree.shaderList[1].emissive = rgb(55, 55, 55)
      mAppleTree.shaderList[2].emissive = rgb(55, 55, 55)
      mAppleTreeArrow = w.cloneModelFromCastmember("AppleTreeArrowClone", "ArrowAppleTree_000", member(1, "w3D"))
      mAppleTree.addChild(mAppleTreeArrow)
      mAppleTree.scale(0.16, 0.16, 0.16)
      mAppleTree.transform.position = vector(-100, 20, 0)
      gAppleTree = w.newGroup("BaleGroup")
      gAppleTree.addChild(mAppleTree)
      mItemList.add(gAppleTree)
      gItems.addChild(gAppleTree)
      mRiver = w.cloneModelFromCastmember("RiverClone", "TILE_RiverSBend_001", member(1, "w3D"))
      w.deleteModel(mRiver.child[2].name)
      mRiver.child[2].removeFromWorld()
      mRiver.transform.position = vector(-90, 45, 0)
      mRiver.scale(0.015, 0.015, 0.015)
      mRiver.rotate(10, 0, 0)
      gRiver = w.newGroup("RiverGroup")
      gRiver.addChild(mRiver)
      mItemList.add(gRiver)
      gItems.addChild(gRiver)
      mApplePress = w.cloneModelFromCastmember("ApplePressClone", "ApplePress_000", member(1, "w3D"))
      mApplePressArm = w.cloneModelFromCastmember("ApplePressArmClone", "ApplePressArm_001", member(1, "w3D"))
      mApplePressArrow = w.cloneModelFromCastmember("ApplePressArrowClone", "ArrowApplePress_000", member(1, "w3D"))
      mApplePressArmArrow = w.cloneModelFromCastmember("ApplePressArmArrowClone", "ArrowApplePressArm_000", member(1, "w3D"))
      mBFS = w.cloneModelFromCastmember("BFSClone", member(8, "w3D").model(1).name, member(8, "w3D"))
      mApplePress.addChild(mApplePressArm)
      mApplePress.addChild(mApplePressArrow)
      mApplePressArm.addChild(mApplePressArmArrow)
      gBFS = w.newGroup("BFSGroup")
      gBFS.addChild(mBFS)
      mApplePressArm.addChild(gBFS)
      gBFS.translate(0, 60, -120)
      gBFS.rotate(0, 90, 0)
      gBFS.scale(1.19999999999999996, 1.19999999999999996, 1.19999999999999996)
      mBFS.translate(0, -40, 0)
      mApplePressArrow.scale(0.80000000000000004, 0.80000000000000004, 0.80000000000000004)
      mApplePressArrow.translate(0, 20, 0)
      mApplePress.transform.position = vector(-90, 35, 0)
      mApplePress.scale(0.20000000000000001, 0.20000000000000001, 0.20000000000000001)
      mApplePress.rotate(10, 0, 0)
      gApplePress = w.newGroup("ApplePressGroup")
      gApplePress.addChild(mApplePress)
      mItemList.add(gApplePress)
      gItems.addChild(gApplePress)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(79, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(78, "Textures")))
      tTextList[2].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox3Texture", #fromCastMember, member(80, "Textures")))
      tTextList[3].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox4Texture", #fromCastMember, member(81, "Textures")))
      tTextList[4].renderFormat = #rgba8888
    6:
      mMother.bonesPlayer.blendTime = 0
      mMother.bonesPlayer.queue("MotherFearMotion", 1)
      mMother.bonesPlayer.playNext()
      mTimmy = w.cloneModelFromCastmember("TimmyClone", member(9, "w3D").model(1).name, member(9, "w3D"))
      mTimmy.transform.position = vector(-100, 25, 0)
      mTimmy.scale(1.10000000000000009, 1.10000000000000009, 1.10000000000000009)
      mTimmy.rotate(0, 180, 0)
      gTimmy = w.newGroup("TimmyGroup")
      gTimmy.addChild(mTimmy)
      mItemList.add(gTimmy)
      gItems.addChild(gTimmy)
      w.cloneMotionFromCastmember("TimmyScaredMotion", member(26, "Motions").motion[2].name, member(26, "Motions"))
      mTimmy.bonesPlayer.blendTime = 0
      mTimmy.bonesPlayer.queue("TimmyScaredMotion", 1)
      mTimmy.bonesPlayer.playNext()
      mBarn = w.cloneModelFromCastmember("BarnObjectsClone", "TILE_BarnObjects_004", member(1, "w3D"))
      mBullBars = w.cloneModelFromCastmember("BullBarsClone", "LandroverBullBars_000", member(1, "w3D"))
      mBarn.addChild(mBullBars)
      mBullBars.transform.position = vector(300, 70, 0)
      mBullBars.scale(4, 4, 4)
      mBarn.scale(0.10000000000000001, 0.10000000000000001, 0.10000000000000001)
      mBarn.rotate(0, 180, 0)
      mBarn.shaderList[9].blend = 0
      mBarn.transform.position = vector(-80, 10, 0)
      gBarn = w.newGroup("BarnGroup")
      gBarn.addChild(mBarn)
      mItemList.add(gBarn)
      gItems.addChild(gBarn)
      mBull = w.cloneModelFromCastmember("BullClone", member(5, "w3D").model(1).name, member(5, "w3D"))
      mBull.transform.position = vector(-90, 10, 0)
      mBull.scale(0.59999999999999998, 0.59999999999999998, 0.59999999999999998)
      mBull.rotate(0, 180, 0)
      gBull = w.newGroup("BullGroup")
      gBull.addChild(mBull)
      mItemList.add(gBull)
      gItems.addChild(gBull)
      w.cloneMotionFromCastmember("BullPawMotion", member(1, "Motions").motion[2].name, member(1, "Motions"))
      mBull.bonesPlayer.blendTime = 0
      mBull.bonesPlayer.queue("BullPawMotion", 1)
      mBull.bonesPlayer.playNext()
      mBarn1 = w.cloneModelFromCastmember("BarnObjectsClone1", "TILE_BarnObjects_004", member(1, "w3D"))
      mBarn1.scale(0.10000000000000001, 0.10000000000000001, 0.10000000000000001)
      mBarn1.rotate(0, 180, 0)
      mBarn1.shaderList[9].blend = 0
      mBarn1.transform.position = vector(-80, 10, 0)
      gBarn1 = w.newGroup("BarnGroup1")
      gBarn1.addChild(mBarn1)
      mItemList.add(gBarn1)
      gItems.addChild(gBarn1)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(82, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(83, "Textures")))
      tTextList[2].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox3Texture", #fromCastMember, member(84, "Textures")))
      tTextList[3].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox4Texture", #fromCastMember, member(92, "Textures")))
      tTextList[4].renderFormat = #rgba8888
    7:
      mFlockSheep = w.cloneModelFromCastmember("FlockSheepClone", member(6, "w3D").model(1).name, member(6, "w3D"))
      w.cloneMotionFromCastmember("FlockSheepFearMotion", member(4, "Motions").motion[2].name, member(4, "Motions"))
      w.cloneMotionFromCastmember("FlockSheepGrinMotion", member(5, "Motions").motion[2].name, member(5, "Motions"))
      w.cloneMotionFromCastmember("FlockSheepLookAroundMotion", member(6, "Motions").motion[2].name, member(6, "Motions"))
      w.cloneMotionFromCastmember("FlockSheepLookUpMotion", member(7, "Motions").motion[2].name, member(7, "Motions"))
      mFlockSheep.bonesPlayer.blendTime = 0
      mFlockSheep.bonesPlayer.queue("FlockSheepLookAroundMotion", 1)
      mFlockSheep.bonesPlayer.playNext()
      mFlockSheep.transform.position = vector(-50, 20, 0)
      mFlockSheep.scale(0.69999999999999996, 0.69999999999999996, 0.69999999999999996)
      mFlockSheep.rotate(0, 190, 0)
      sheepList = []
      repeat with i = 1 to 5
        sheepList.add(mFlockSheep.clone("FlockSheepClone" & i))
      end repeat
      repeat with i = 1 to 5
        mFlockSheep.addChild(sheepList[i])
        mFlockSheep.child[i].translate(40 * i, 0, 30 * i)
      end repeat
      mFlockSheep.child[1].bonesPlayer.queue("FlockSheepGrinMotion", 1)
      mFlockSheep.child[1].bonesPlayer.playNext()
      mFlockSheep.child[2].bonesPlayer.queue("FlockSheepLookAroundMotion", 1)
      mFlockSheep.child[2].bonesPlayer.playNext()
      mFlockSheep.child[2].bonesPlayer.currentTime = 400
      mFlockSheep.child[3].bonesPlayer.queue("FlockSheepFearMotion", 1)
      mFlockSheep.child[3].bonesPlayer.playNext()
      mFlockSheep.child[4].bonesPlayer.queue("FlockSheepLookUpMotion", 1)
      mFlockSheep.child[4].bonesPlayer.playNext()
      mFlockSheep.child[4].bonesPlayer.currentTime = random(400)
      gFlockSheep = w.newGroup("FlockSheep")
      gFlockSheep.addChild(mFlockSheep)
      mItemList.add(gFlockSheep)
      gItems.addChild(gFlockSheep)
      mBarn = w.cloneModelFromCastmember("BarnObjectsClone", "TILE_BarnObjects_004", member(1, "w3D"))
      mBarn.scale(0.10000000000000001, 0.10000000000000001, 0.10000000000000001)
      mBarn.rotate(0, 180, 0)
      mBarn.shaderList[9].blend = 0
      mBarn.transform.position = vector(-80, 10, 0)
      gBarn = w.newGroup("BarnGroup")
      gBarn.addChild(mBarn)
      mItemList.add(gBarn)
      gItems.addChild(gBarn)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(85, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(88, "Textures")))
      tTextList[2].renderFormat = #rgba8888
    8:
      mLitter1 = w.cloneModelFromCastmember("LitterClone1", "LitterNewPaper_000", member(1, "w3D"))
      mLitter2 = w.cloneModelFromCastmember("LitterClone2", "LitterCan_000", member(1, "w3D"))
      mLitter3 = w.cloneModelFromCastmember("LitterClone3", "LitterCoke_000", member(1, "w3D"))
      mLitter4 = w.cloneModelFromCastmember("LitterClone4", "LitterLager_000", member(1, "w3D"))
      mBFS = w.cloneModelFromCastmember("BullClone", member(8, "w3D").model(1).name, member(8, "w3D"))
      mBFS.addChild(mLitter1)
      mBFS.addChild(mLitter2)
      mBFS.addChild(mLitter3)
      mBFS.addChild(mLitter4)
      mLitter1.transform.position = vector(40, 0, -50)
      mLitter1.rotate(-90, 100, 0)
      mLitter1.scale(0.5, 0.5, 0.5)
      mLitter2.transform.position = vector(-30, 0, 20)
      mLitter2.rotate(-90, 45, 0)
      mLitter2.scale(0.5, 0.5, 0.5)
      mLitter3.transform.position = vector(-50, 0, -20)
      mLitter3.rotate(-90, 0, 0)
      mLitter3.scale(0.5, 0.5, 0.5)
      mLitter4.transform.position = vector(30, 0, 20)
      mLitter4.rotate(-90, 0, 0)
      mLitter4.scale(0.5, 0.5, 0.5)
      mBFS.transform.position = vector(-120, 30, 0)
      mBFS.transform.scale(0.80000000000000004, 0.80000000000000004, 0.80000000000000004)
      mBFS.rotate(0, 190, 0)
      gBFS = w.newGroup("BSFGroup")
      gBFS.addChild(mBFS)
      mItemList.add(gBFS)
      gItems.addChild(gBFS)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(86, "Textures")))
      tTextList[1].renderFormat = #rgba8888
    9:
      mDot = w.cloneModelFromCastmember("DotClone", "RacePoint_000", member(1, "w3D"))
      mArrow = w.cloneModelFromCastmember("ArrowClone", "ArrowRacePoint_000", member(1, "w3D"))
      mDot.addChild(mArrow)
      mDot.transform.scale = vector(0.40000000000000002, 0.40000000000000002, 0.40000000000000002)
      mDot.transform.position = vector(-100, 30, 0)
      mDot.rotate(0, 0, 0)
      gDot = w.newGroup("DotGroup")
      gDot.addChild(mDot)
      mItemList.add(gDot)
      gItems.addChild(gDot)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(89, "Textures")))
      tTextList[1].renderFormat = #rgba8888
    10:
      mBigTop = w.cloneModelFromCastmember("BigTopObjectsClone", "TILE_BigTopObjects_000", member(1, "w3D"))
      mCannon = w.cloneModelFromCastmember("CannonClone", "LandroverCannon_003", member(1, "w3D"))
      mBigTop.addChild(mCannon)
      mCannon.transform.position = vector(400, 30, -400)
      mCannon.scale(4, 4, 4)
      mCannon.transform.rotation = vector(0, 70, 0)
      mBigTop.scale(0.07000000000000001, 0.07000000000000001, 0.07000000000000001)
      mBigTop.rotate(0, 180, 0)
      mBigTop.transform.position = vector(-80, 5, 0)
      gBigTop = w.newGroup("BarnGroup")
      gBigTop.addChild(mBigTop)
      mItemList.add(gBigTop)
      gItems.addChild(gBigTop)
      gKeys = w.newGroup("KeysGroup")
      gKeysHandle = w.newGroup("KeysHandleGroup")
      gKeysHandle.addChild(gKeys)
      mItemList.add(gKeysHandle)
      mCursorKey1 = w.cloneModelFromCastmember("CursorKeyClone1", "CursorKey_000", member(10, "w3D"))
      mCursorKey2 = mCursorKey1.clone("CursorKeyClone2")
      mCursorKey3 = mCursorKey1.clone("CursorKeyClone3")
      mCursorKey4 = mCursorKey1.clone("CursorKeyClone4")
      mSpaceBarKey = w.cloneModelFromCastmember("SpaceBarKeyClone", "SpaceBarKey_000", member(10, "w3D"))
      mTarget = w.cloneModelFromCastmember("TargetClone", "LandroverCannonTarget_000", member(1, "w3D"))
      mSplat = w.cloneModelFromCastmember("SplatClone", "LandroverCannonPieSplat_000", member(1, "w3D"))
      mTarget.addChild(mSplat)
      mSplat.transform.position = vector(0, 40, 0)
      mSplat.rotate(90, 0, 0)
      mSplat.transform.scale = vector(1.5, 1.5, 1.5)
      mTarget.transform.position = vector(90, 60, 0)
      mTarget.scale(0.5, 0.5, 0.5)
      mTarget.rotate(0, 180, 0)
      gKeys.addChild(mCursorKey1)
      gKeys.addChild(mCursorKey2)
      gKeys.addChild(mCursorKey3)
      gKeys.addChild(mCursorKey4)
      gKeys.addChild(mSpaceBarKey)
      gKeys.addChild(mTarget)
      mCursorKey2.translate(30, 0, 0)
      mCursorKey2.transform.scale = vector(-1, 1, 1)
      mCursorKey3.translate(15, 0, 0)
      mCursorKey3.rotate(0, -90, 0)
      mCursorKey3.transform.scale = vector(1, 1, -1)
      mCursorKey4.translate(15, 0, 15)
      mCursorKey4.rotate(0, 90, 0)
      mSpaceBarKey.translate(100, 0, 0)
      gKeys.rotate(-30, 180, 0)
      gKeys.transform.position = vector(-50, 24, 0)
      gKeys.scale(0.5, 0.5, 0.5)
      gItems.addChild(gKeysHandle)
      tTextList.add(w.newTexture("TextBox1Texture", #fromCastMember, member(90, "Textures")))
      tTextList[1].renderFormat = #rgba8888
      tTextList.add(w.newTexture("TextBox2Texture", #fromCastMember, member(91, "Textures")))
      tTextList[2].renderFormat = #rgba8888
  end case
  gItems.worldPosition.x = -500
  repeat with i = 2 to mItemList.count
    mItemList[i].removeFromWorld()
  end repeat
  startTicks = the ticks
  changeAnimStartTicks = the ticks
  animLength = 100
  animState = 0
  lookOffset = vector(0, 0, 0)
  blinkStartTicks = the ticks
  blinkInterval = 100
  prevMouseOver = 0
  cam = sprite(1).camera
  cam.fieldOfView = 20
  cam.transform.rotation = vector(-5, 12, 0)
  cam.worldPosition = vector(0, 70, 300)
  tempTexture = w.newTexture("MotherTitleTexture", #fromCastMember, member(70, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(30, 10), 0)
  oTabList = []
  tabPrevMouseOverList = []
  tempTexture = w.newTexture("StartButtonTexture", #fromCastMember, member(71, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(600, 453), 0)
  oStartButton = cam.overlay.count
  tempTexture = w.newTexture("BackButtonTexture", #fromCastMember, member(98, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(42, 453), 0)
  oBackButton = cam.overlay.count
  cam.addOverlay(tTextList[1], point(120, 340), 0)
  oTextBox = cam.overlay.count
  cam.overlay[oTextBox].blend = 0
  tabPrevMouseOverList.add(0)
  tempTexture = w.newTexture("menuBackdropTexture", #fromCastMember, member(69, "Textures"))
  tempTexture.renderFormat = #rgba8888
  nmr = w.newModelResource("MenuBackdropResource", #plane, #back)
  mBackdrop = w.newModel("MenuBackdropModel", nmr)
  mBackdrop.shaderList[1].texture = tempTexture
  mBackdrop.scale(300, 300, 300)
  mBackdrop.worldPosition = vector(-100, 60, -100)
  mBackdrop.shaderList[1].textureTransform.scale(0.20000000000000001, 0.20000000000000001, 0.20000000000000001)
  if g_Muted = 0 then
    puppetSound(1, member(17, "Sfx"))
    puppetSound(3, member(29, "Sfx"))
  end if
  startTicks = the ticks
end

on exitFrame
  if (g_Muted = 0) and (sound(1).member <> member(17, "Sfx")) and (g_NextFramePlease = g_NextFrame) then
    puppetSound(1, member(17, "Sfx"))
  end if
  theMouseDown = the mouseDown
  gItems.worldPosition = gItems.worldPosition * 0.90000000000000002
  if cam.overlay[oTextBox].blend < 100 then
    cam.overlay[oTextBox].blend = cam.overlay[oTextBox].blend + 2
  end if
  case g_MissionID of
    1:
      if (the ticks - keyStartTicks) > 20 then
        keyStartTicks = the ticks
        keyID = keyID + 1
        if keyID > 4 then
          keyID = 1
        end if
        case keyID of
          1:
            mCursorKey1.transform.position.y = 0
            mCursorKey2.transform.position.y = -4
            mCursorKey3.transform.position.y = 0
            mCursorKey4.transform.position.y = 0
          2:
            mCursorKey1.transform.position.y = -4
            mCursorKey2.transform.position.y = 0
            mCursorKey3.transform.position.y = 0
            mCursorKey4.transform.position.y = 0
          3:
            mCursorKey1.transform.position.y = 0
            mCursorKey2.transform.position.y = 0
            mCursorKey3.transform.position.y = -4
            mCursorKey4.transform.position.y = 0
          4:
            mCursorKey1.transform.position.y = 0
            mCursorKey2.transform.position.y = 0
            mCursorKey3.transform.position.y = 0
            mCursorKey4.transform.position.y = -4
        end case
      end if
      if (the ticks - startTicks) > 250 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    2:
      if (the ticks - startTicks) > 250 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    3:
      if (the ticks - startTicks) > 400 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    4:
      if (the ticks - keyStartTicks) > 20 then
        keyStartTicks = the ticks
        keyID = keyID + 1
        if keyID > 2 then
          keyID = 1
        end if
        case keyID of
          1:
            mSpaceBarKey.transform.position.y = 0
          2:
            mSpaceBarKey.transform.position.y = -4
        end case
      end if
      mItemList[2].child[1].child[1].rotate(0, 4, 0)
      mItemList[2].child[1].child[1].shaderList[3].textureTransform.translate(-0.02, 0, 0)
      if (the ticks - startTicks) > 500 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    5:
      mItemList[4].child[1].child[1].rotate(0, 2, 0)
      mItemList[4].child[1].child[1].child[2].child[1].rotate(-4, 0, 0, #parent)
      if (the ticks - startTicks) > 400 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    6:
      if (the ticks - startTicks) > 400 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    7:
      if (the ticks - startTicks) > 400 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        startTicks = the ticks
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
    8:
      if (the ticks - startTicks) > 400 then
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        startTicks = the ticks
      end if
    9:
      if (the ticks - startTicks) > 400 then
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        startTicks = the ticks
      end if
    10:
      if (the ticks - keyStartTicks) > 20 then
        keyStartTicks = the ticks
        keyID = keyID + 1
        if keyID > 2 then
          keyID = 1
        end if
        case keyID of
          1:
            mSpaceBarKey.transform.position.y = 0
          2:
            mSpaceBarKey.transform.position.y = -4
        end case
      end if
      if (the ticks - startTicks) > 400 then
        gItems.worldPosition.x = -500
        itemID = itemID + 1
        if itemID > mItemList.count then
          itemID = 1
          startButtonFlashing = 1
        end if
        startTicks = the ticks
        cam.overlay[oTextBox].blend = 0
        cam.overlay[oTextBox].source = tTextList[itemID]
        startTicks = the ticks
        repeat with i = 1 to mItemList.count
          if i <> itemID then
            mItemList[i].removeFromWorld()
            next repeat
          end if
          gItems.addChild(mItemList[i])
          mItemList[i].transform.position = vector(0, 0, 0)
        end repeat
      end if
  end case
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
      g_NextFramePlease = 10
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
  tempDiff = the mouseLoc - cam.overlay[oStartButton].loc
  if (abs(tempDiff.locH) < 128) and (abs(tempDiff.locV) < 32) then
    if startButtonPrevMouseOver = 0 then
      if g_Muted = 0 then
        puppetSound(2, member(24, "Sfx"))
      end if
    end if
    startButtonPrevMouseOver = 1
    if theMouseDown = 1 then
      cam.overlay[oStartButton].scale = 0.90000000000000002
      cam.overlay[oStartButton].regPoint = point(128, 32) - point(14, 2)
    else
      cam.overlay[oStartButton].scale = 1.10000000000000009
      cam.overlay[oStartButton].regPoint = point(128, 32) - point(-14, -2)
    end if
    if (theMouseDown = 0) and (prevMouseDown = 1) then
      g_NextFramePlease = 19
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
    startButtonPrevMouseOver = 0
    cam.overlay[oStartButton].scale = 1
    cam.overlay[oStartButton].regPoint = point(128, 32)
  end if
  if startButtonFlashing = 1 then
    flashCounter = flashCounter + 1
    if flashCounter > 10 then
      flashCounter = 0
    end if
    if flashCounter <= 6 then
      cam.overlay[oStartButton].blend = 100
    else
      cam.overlay[oStartButton].blend = 40
    end if
  end if
  mBackdrop.shaderList[1].textureTransform.translate(0.001, 0.001, 0)
  mMother.worldPosition.x = mMother.worldPosition.x * 0.90000000000000002
  if (the ticks - changeAnimStartTicks) > animLength then
    animState = animState + 1
    changeAnimStartTicks = the ticks
    if animState > 3 then
      animState = 1
    end if
    case animState of
      1:
        mMother.bonesPlayer.blendTime = 200
        mMother.bonesPlayer.queue("MotherNodMotion", 1)
        mMother.bonesPlayer.playNext()
        if g_Muted = 0 then
        end if
        animLength = 400
      2:
        mMother.bonesPlayer.blendTime = 300
        mMother.bonesPlayer.queue("MotherSmileMotion", 1)
        mMother.bonesPlayer.playNext()
        if g_Muted = 0 then
        end if
        animLength = 300
      3:
        mMother.bonesPlayer.blendTime = 300
        mMother.bonesPlayer.queue("MotherSmileMotion", 1)
        mMother.bonesPlayer.playNext()
        lookOffset = vector(0, 0, 0)
        if g_Muted = 0 then
          puppetSound(3, member(29, "Sfx"))
        end if
        animLength = 400
    end case
  end if
  prevMouseDown = theMouseDown
end

on mouseDown me
end
