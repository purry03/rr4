property gPlayerCar, mPlayerCar, mTireFL, mTireFR, mTireBL, mTireBR, mBonnet, mDoorL, mDoorR, mBoot, mPlayerCarMirror, velVector, rotVector, rotVelVector, steer, gCageList, imbedList, imbedCapList, isectPosList, isectNormList, toCenterList, midAir, hoist, tempFactor, gripVector, throttle, hoistCap, onTileID, visTileCollisionList, igNitroRacePosList, igNitroStagePosList, firstIgNitro, oRadarPointPod, oRadarRacePod, opponentDotID, opponentState, opponentTargetPos, prevOpponentVelVector, mOpponentShellList, enterFerrisCarID, prevCurrentScore, gOpponentCar, mOpponentCar, mOpponentTireFR, mOpponentTireFL, mOpponentTireBR, mOpponentTireBL, opponentVelVector, opponentSteer, opponentRotVector, opponentRotVelVector, opponentHoist, opponentPrevMidAir, opponentMidAir, opponentSpawned, opponentTargetVel, opponentThrottle, opponentOnTileIDList, opponentOnTile, opponentTileRowList, opponentTileColList, opponentImbedRotList, opponentImbedIDList, gOpponentCageList, opponentImbedList, opponentImbedCapList, opponentIsectPosList, opponentisectNormList, opponentToCenterList, opponentGripVector, opponentOnTileID, opponentTileRotList, gOpponentCage, w, cam, camY, prevSpaceDown, yRotVel, prevMouseDown, prevMouseH, prevMouseV, openUp, camGroundHeight, mGround, mShadow, mSkyBox, tileRows, tileCols, tileIDList, tileRowList, tileColList, totalTiles, tilesIDList, imbedIDList, onTile, onTileX, onTileRow, onTileCol, onTilez, prevVelVector, prevRotVector, partStateList, partRotList, partRotVelList, camAim, camState, mTilePool, mTileResourceList, mVisTileList, minVisCol, maxVisCol, visTiles, visTileIDList, visOnTile, minVisRow, maxVisRow, visTileDist, gVisTiles, visTileRows, visTileCols, prevOnTileCol, prevOnTileRow, prevOnTile, shadowsOn, camRange, camHeight, tileRotList, imbedRotList, visTileRotList, prevPlayerCarPos, dustObstacle, prevThrottle, rollVel, prevRollVel, prevMidAir, keyPressedList, prevKeyPressedList, shuffleAngle, radians, gameStartTicks, gameState, mShower, tCanalWater, mShellList, gPlayerCage, playerCarID, engineSfxList, mIgNitroBlastL, mIgNitroBlastR, nmrBurnSmoke, soundMemberList, nmrTireSmoke, tireSmokeToggle, mFerrisWheel, mFerrisAxleList, mFerrisCageList, ferrisSwing, ferrisSwingVel, ferrisRot, ferrisRotVel, prevferrisRotVel, ferrisCageCarIDList, ferrisState, ferrisTargetRot, gCarPlaceHolder, gTirePlaceHolder, ferrisCurrentCageID, ferrisCalcAngle, ferrisPrevRot, ferrisRotAmount, prevFerrisRotAmount, prevBoosting, ferrisWheelSpawned, carDamageList, carBoostList, repairState, mRixGarageDoor, mColorCans, tColorCansPaint1, tColorCansPaint2, tColorSplash, prevDoorOpenFactor, colorTankState, colorSplashPercent, pointsFlagRotList, pointsFlagStartTicks, carColorIDList, colorSplashID, colorRot, colorPrevRot, colorTargetRot, colorCalcAngle, mIgNitroList, mIgNitro, gIgNitro, igNitroRot, tPointerArrows, mRaceFlag, tRaceFlagArrows, tRaceFlagLights, pointsFlagPosList, tPointsFlagArrows, mPointsFlag, oRadarRing, oRadarFix, oRadarRace, oRadarStage, mPointPodList, mPointPod, mNextPointPod, pointPodPointer, stagePodPosList, stageState, raceState, raceLightPhase, raceLightStartTicks, currentStage, raceFlagStartTicks, lastRecordedPos, raceFlagPosList, raceFlagRotList, mRacePod, mNextRacePod, racePodPosList, currentRace, racePodPointer, damageStartTicks, prevDamagePercent, prevCurrentRace, nextDotIDList, prevDotIDList, dotPosList, redTrafficLightIDList, greenTrafficLightIDList, tileDotIDList, checkPointIDList, redDotPosList, prevRedDotIDList, nextRedDotIDList, trafficLightPhase, trafficLightStartTicks, mTrafficCar, trafficSpeedList, trafficDotIDList, dotOccupiedList, trafficTargetPosList, trafficTargetSpeedList, mapIDList, trafficCullDist, trafficSpawnDist, trafficSpawnDotIDList, mTrafficList, trafficStateList, trafficSpawningDotIDList, trafficSpawnedCounter, trafficSpawnedLimit, trafficDotSpawnTicksList, mousePosVector, gPlane, mPlane, planeSpeed, planeState, airPortOrigin, planeYRot, planeTilt, planeBank, planeStartTicks, mRadar, gPlaneCollider, engineState, soundCompletePercent, upPressed, prevUpPressed
global g_Paused, g_Muted, g_DamagePercent, g_FPSFactor, g_FPSFactorDec, g_FPSFactorFrac, g_MissionID, g_BoostPercent, g_FadeToBlack, g_NextFramePlease, g_SaveGameName, g_CurrentScore, g_MissionHiScoreList, g_RaceBestTimeList, g_GameOverID, g_TimerState, g_CarsOwnedList, g_CarsColorIDList, g_CarsDamageList, g_CarsIgNitroList, g_NumberRacesWon, g_Points, g_NumberCarsUnlocked, g_RacesNoDamage, g_MidAirTime

on keyUp
  case the keyCode of
    49:
  end case
  if the keyCode = 12 then
  end if
end

on beginSprite
  w = member(1, "w3D")
  w.resetWorld()
  lastRecordedPos = vector(-10000000, -10000000, -100000)
  mShower = w.model("trafficDot_001")
  mShower.removeFromWorld()
  mTrafficCar = w.model("TrafficCar_000")
  mTrafficCar.removeFromWorld()
  member("MemoPad", "Textures").text = EMPTY
  flightPathDotPosList = []
  checkPointDotIDList = []
  mCheckPointList = []
  mCarList = []
  nextDotIDList = []
  prevDotIDList = []
  dotOccupiedList = []
  trafficDotIDList = []
  trafficTargetPosList = []
  trafficTargetSpeedList = []
  trafficSpeedList = []
  redTrafficLightIDList = []
  greenTrafficLightIDList = []
  mRedTrafficLightList = []
  mGreenTrafficLightList = []
  redDotCounter = 1
  redLineCounter = 1
  dotCounter = 1
  lineCounter = 1
  checkPointCounter = 1
  mCheckPointList = []
  checkPointIDList = []
  dotID = 0
  prevDotID = 0
  nextDotID = 0
  zoomFactor = 1.0
  zoomVel = 0.0
  hitherVel = 0.0
  trafficSpawnDist = 4000
  trafficCullDist = 4500
  trafficSpawnDotIDList = []
  mTrafficList = []
  trafficStateList = []
  trafficDotIDList = []
  trafficSpawningDotIDList = []
  trafficSpawnedCounter = 0
  trafficSpawnedLimit = 0
  trafficDotSpawnedTicksList = []
  camTileID = 1
  nextDotIDList = []
  prevDotIDList = []
  dotPosList = []
  redDotPosList = []
  nextRedDotIDList = []
  prevRedDotIDList = []
  redTrafficIDList = []
  greenTrafficIDList = []
  tileDotIDList = []
  checkPointIDList = []
  redTrafficLightCounter = 1
  greenTrafficLightCounter = 1
  trafficLightStartTicks = the ticks
  trafficLightPhase = 1
  engineState = 0
  soundCompletePercent = 1.0
  radians = 180.0 / PI
  tileCols = 12
  tileRows = 12
  totalTiles = tileCols * tileRows
  tileIDList = []
  tileRowList = []
  tileColList = []
  tileIDList[totalTiles] = 0
  onTileX = 0
  onTilez = 0
  onTile = 0
  onTileID = 0
  onTileCol = 1
  onTileRow = 1
  prevOnTileCol = 1
  prevOnTileRow = 1
  prevOnTile = 0
  visTileDist = 2
  visTileCols = 1 + (visTileDist * 2)
  visTileRows = 1 + (visTileDist * 2)
  visTiles = visTileRows * visTileCols
  visOnTile = integer(1 + ((visTiles - 1) * 0.5))
  visTileRotList = []
  visTileRotList[visTiles] = 0
  mVisTileList = []
  visTileCollisionList = []
  visTileIDList = []
  visTileIDList[visTiles] = 0
  tileIDList = [27, 19, 5, 9, 9, 9, 9, 11, 11, 11, 11, 11, 19, 13, 17, 3, 4, 3, 11, 11, 2, 11, 2, 12, 16, 14, 17, 11, 10, 11, 11, 2, 11, 2, 2, 8, 19, 14, 17, 6, 7, 6, 6, 7, 7, 7, 6, 8, 18, 15, 28, 6, 7, 6, 6, 7, 7, 7, 6, 8, 19, 15, 28, 35, 36, 35, 35, 36, 35, 37, 35, 8, 18, 15, 28, 2, 2, 2, 20, 2, 2, 2, 2, 8, 18, 15, 28, 2, 2, 2, 2, 2, 2, 2, 2, 12, 19, 14, 34, 23, 23, 23, 21, 23, 23, 22, 2, 2, 19, 14, 29, 25, 23, 24, 23, 23, 23, 25, 2, 29, 19, 13, 33, 22, 25, 23, 22, 25, 26, 23, 2, 30, 27, 27, 31, 32, 32, 32, 32, 32, 32, 32, 32, 31]
  tileRotList = [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 2, 1, 0, 0, 2, 0, 0, 0, 2, 1, 1, 2, 3, 1, 1, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2]
  keyPressedList = [0, 0, 0, 0, 0]
  prevKeyPressedList = [0, 0, 0, 0, 0]
  shuffleAngle = 120
  targetShuffleAngle = 120
  shuffleBlend = 0
  prevPlayerCarPos = vector(0, 0, 0)
  appleStartTicks = the ticks
  prevArmToRoverVector = vector(0, 0, 0)
  pulpAngleCounter = 0
  gameStartTicks = the ticks
  gameState = 1
  obstaclesPassed = 0
  timmyRescued = 0
  pointerArrowFrame = 130
  if getRendererServices().renderer = #software then
    shadowsOn = 0
  else
    shadowsOn = 1
  end if
  partStateList = [0, 0, 0, 0, 0, 0, 0, 0]
  partRotVelList = [0, 0, 0, 0, 0, 0, 0, 0]
  partRotList = [0, 0, 0, 0, 0, 0, 0, 0]
  openUp = 0
  velVector = vector(0, 0, 0)
  rotVector = vector(0, 0, 0)
  rotVelVector = vector(0, 0, 0)
  prevVelVector = vector(0, 0, 0)
  prevRotVector = vector(0, 0, 0)
  camGroundHeight = 0
  steer = 0
  throttle = 0
  prevThrottle = 0
  yRotVel = 0
  rollVel = 0
  prevRollVel = 0
  prevMouseV = the mouseV
  prevMouseH = the mouseH
  prevSpaceDown = 0
  barnState = 0
  barnStartTicks = the ticks
  addOnID = 0
  barnAddOnID = 0
  lookingForwards = 0
  doLoadData()
  doSetupigNitro()
  doSetupShells()
  doSetupPlayerCar()
  doSetUpTiles()
  doSetupBurnSmoke()
  doSetupTraffic()
  doSetupTrafficLights()
  doTraffic()
  doSetupOpponent()
  doSetupTallObjects()
  doSetupPlane()
  doSetupFerrisWheel()
  doSetupRixRapidRepairs()
  doSetupColorTank()
  doSetupRaceFlag()
  doSetupPointsFlag()
  doSetupRadar()
  doSetupPointPods()
  doSetupRacePods()
  if g_Muted = 0 then
    puppetSound(5, member(15, "Sfx"))
    soundMemberList[5] = member(15, "Sfx")
  end if
end

on enterFrame
  if g_Paused = 0 then
    doEngineSounds()
    doTiles()
    doTraffic()
    doTrafficLights()
    doOpponent()
    doRayCasting()
    doControls()
    doGravity()
    doFriction()
    doTireGrip()
    doCollisions()
    doPlane()
    doFerrisWheel()
    doRixRapidRepairs()
    doColorTank()
    doIgNitro()
    doRaceFlag()
    doPointsFlag()
    doRadar()
    doPointPods()
    doRacePods()
    doBodyRoll()
    doupdate()
    doParts()
    doAnims()
    doCam()
    prevThrottle = throttle
    prevMouseH = the mouseH
    prevMouseV = the mouseV
    prevMouseDown = the mouseDown
    prevSpaceDown = keyPressed(49)
    prevRotVector = rotVector.duplicate()
    prevVelVector = velVector.duplicate()
    prevOnTileCol = onTileCol
    prevOnTileRow = onTileRow
    prevOnTile = onTile
    prevMidAir = midAir
    prevPlayerCarPos = gPlayerCar.worldPosition.duplicate()
    prevKeyPressedList = keyPressedList
    prevDamagePercent = g_DamagePercent
    prevCurrentRace = currentRace
    prevOpponentVelVector = opponentVelVector
    prevCurrentScore = g_CurrentScore
  end if
end

on doSetupPointsFlag
  stageState = 0
  currentStage = 1
  pointsFlagStartTicks = 0
  mPointsFlag = w.model("PointsFlag_001")
  mPointsFlag.worldPosition = vector(11800, 0, 4000)
  pointsFlagPosList = [vector(33854.87000000000261934, -5.55999999999999961, 31742.90999999999985448), vector(25010.29000000000087311, 14.4399999999999995, 29545.83000000000174623), vector(11732.23999999999978172, -5.55999999999999961, 3675.67000000000007276), vector(5323.31999999999970896, -5.55999999999999961, 3401.17999999999983629), vector(32422.27000000000043656, 109.57999999999999829, 782.60000000000002274), vector(27506.40000000000145519, -5.54999999999999982, 39883.88999999999941792), vector(2727.48999999999978172, -5.55999999999999961, 37504.38999999999941792), vector(12052.5499999999992724, -5.55999999999999961, 40998.27999999999883585)]
  pointsFlagRotList = [vector(0, 180, 0), vector(0, 90, 0), vector(0, 0, 0), vector(0, 90, 0), vector(0, 90, 0), vector(0, 180, 0), vector(0, 90, 0), vector(0, 0, 0)]
  tPointsFlagArrows = mPointsFlag.shaderList[5]
end

on doPointsFlag
  case raceLightPhase of
    0:
      if tRaceFlagLights.textureTransform.position.x > -0.0 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    1:
      if tRaceFlagLights.textureTransform.position.x > -0.25 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    2:
      if tRaceFlagLights.textureTransform.position.x > -0.5 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    3:
      if tRaceFlagLights.textureTransform.position.x > -0.75 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
  end case
  case stageState of
    0:
      currentStage = 0
      tempMinMag = 100000000000000000.0
      repeat with i = 1 to pointsFlagPosList.count
        tempDiff = gPlayerCar.worldPosition - pointsFlagPosList[i]
        tempMag = tempDiff.magnitude
        if tempMag < tempMinMag then
          currentStage = i
          tempMinMag = tempMag
        end if
      end repeat
      mPointsFlag.worldPosition = pointsFlagPosList[currentStage]
      mPointsFlag.transform.rotation = pointsFlagRotList[currentStage]
      if raceState > 0 then
        tPointsFlagArrows.textureTransform.position = vector(0, 0, 0)
      else
        tPointsFlagArrows.textureTransform.translate(0, -0.01, 0)
      end if
      diffVector = mPointsFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
      if (diffVector.magnitude < 600) and (raceState = 0) then
        tempXAxis = -mPointsFlag.transform.xAxis
        tempZAxis = -mPointsFlag.transform.zAxis
        tempCarZAxis = mPlayerCar.getWorldTransform().zAxis
        tempXDist = tempXAxis.dot(diffVector)
        tempZDist = tempZAxis.dot(diffVector)
        tempForward = tempZAxis.dot(tempCarZAxis)
        tempForwardVel = tempZAxis.dot(velVector)
        prevDiffVector = mPointsFlag.worldPosition - gPlayerCar.worldPosition
        tempPrevZDist = tempZAxis.dot(prevDiffVector)
        if (tempXDist > 0) and (tempXDist < 250) and (tempZDist > 0) and (tempZDist < 400) and (tempForward > 0.75) then
          if (tempZDist < 100) and (tempForwardVel > 0) and (tempPrevZDist >= 100) then
            pushVector = -tempZAxis * (100 - tempZDist)
            pushVector = pushVector - (tempXAxis * (160 - tempXDist) * 0.10000000000000001)
            pointPodPointer = 1
            velVector = velVector + (pushVector / g_FPSFactor)
            sprite(1).doPanel(18, rgb(255, 255, 255), 0)
            case currentStage of
              1:
                sprite(1).timeList = [0, 0, 0, 0, 2]
              2:
                sprite(1).timeList = [0, 0, 0, 0, 2]
              3:
                sprite(1).timeList = [0, 0, 0, 0, 3]
              4:
                sprite(1).timeList = [0, 0, 0, 0, 4]
              5:
                sprite(1).timeList = [0, 0, 0, 0, 5]
                trafficSpawnedLimit = 20
              6:
                sprite(1).timeList = [0, 0, 0, 0, 5]
              7:
                sprite(1).timeList = [0, 0, 0, 0, 5]
              8:
                sprite(1).timeList = [0, 0, 0, 0, 4]
            end case
            g_TimerState = 0
            sprite(1).updateTimer()
            g_CurrentScore = 0
            stageState = 1
            if g_Muted = 0 then
              puppetSound(1, member(26, "Sfx"))
              soundMemberList[1] = member(26, "Sfx")
            end if
            raceLightStartTicks = the ticks
            raceLightPhase = 1
            tRaceFlagLights.textureTransform.position.x = 0.0
            tPointsFlagArrows.textureTransform.position = vector(0, 0, 0)
            mPointPod.addToWorld()
            mPointPod.worldPosition = stagePodPosList[currentStage][pointPodPointer]
            mNextPointPod.addToWorld()
            mNextPointPod.worldPosition = stagePodPosList[currentStage][pointPodPointer + 1]
            tempPos = mNextPointPod.worldPosition
            tempPos.y = mPointPod.worldPosition.y
            mPointPod.pointAt(tempPos)
            tempPos = mPointPod.worldPosition
            tempPos.y = mNextPointPod.worldPosition.y
            mNextPointPod.pointAt(tempPos)
            if currentStage = 8 then
              gPlane.worldPosition = airPortOrigin + vector(0, 1000, 70000)
              planeTilt = 0
              planeBank = 0
              planeSpeed = 60
              planeYRot = 180
              gPlane.transform.rotation = vector(0, planeYRot, 0)
              planeState = 8
            end if
            repeat with i = 1 to mIgNitroList.count
              if i <= igNitroStagePosList[currentStage].count then
                mIgNitroList[i].visibility = #front
                mIgNitroList[i].worldPosition = igNitroStagePosList[currentStage][i] + vector(0, 55, 0)
                next repeat
              end if
              mIgNitroList[i].visibility = #none
            end repeat
          end if
        end if
      end if
    1:
      diffVector = mPointsFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
      if diffVector.magnitude < 600 then
        tempXAxis = -mPointsFlag.transform.xAxis
        tempZAxis = -mPointsFlag.transform.zAxis
        tempCarZAxis = mPlayerCar.getWorldTransform().zAxis
        tempXDist = tempXAxis.dot(diffVector)
        tempZDist = tempZAxis.dot(diffVector)
        tempForward = tempZAxis.dot(tempCarZAxis)
        tempForwardVel = tempZAxis.dot(velVector)
        tempTwist = tempXAxis.dot(tempCarZAxis)
        if (tempXDist > 0) and (tempXDist < 250) and (tempZDist > 0) and (tempZDist < 400) and (tempForward > 0.75) then
          if (tempZDist < 100) and (tempForwardVel > 0) then
            pushVector = -tempZAxis * (100 - tempZDist)
            velVector = velVector + (pushVector / g_FPSFactor)
          end if
          rotVector.y = rotVector.y - (2 * tempTwist)
        else
          sprite(1).doPanel(22, rgb(255, 255, 255), 200)
          stageState = 0
          raceLightPhase = 0
          mPointPod.removeFromWorld()
          mNextPointPod.removeFromWorld()
          repeat with i = 1 to mIgNitroList.count
            mIgNitroList[i].visibility = #none
          end repeat
        end if
        if (the ticks - raceLightStartTicks) > 60 then
          raceLightStartTicks = the ticks
          raceLightPhase = raceLightPhase + 1
          sprite(1).doPanel(17 + raceLightPhase, rgb(255, 255, 255), 0)
          if raceLightPhase >= 3 then
            g_TimerState = 1
            pointPodPointer = 1
            stageState = 2
            sprite(1).doPanel(20, rgb(255, 255, 255), 100)
          end if
        end if
      end if
    2:
      if pointPodPointer > stagePodPosList[currentStage].count then
        g_TimerState = 2
        raceLightPhase = 0
        sprite(1).doPanel(21, rgb(255, 255, 255), 0)
        stageState = 3
        pointsFlagStartTicks = the ticks
        repeat with i = 1 to mIgNitroList.count
          mIgNitroList[i].visibility = #none
        end repeat
        if g_Muted = 0 then
          puppetSound(1, member(27, "Sfx"))
          soundMemberList[1] = member(27, "Sfx")
        end if
        trafficSpawnedLimit = 5
      else
        if g_TimerState = -1 then
          if g_Muted = 0 then
            puppetSound(1, member(28, "Sfx"))
            soundMemberList[1] = member(28, "Sfx")
          end if
          mPointPod.removeFromWorld()
          mNextPointPod.removeFromWorld()
          repeat with i = 1 to mIgNitroList.count
            mIgNitroList[i].visibility = #none
          end repeat
          stageState = 6
          sprite(1).doPanel(25, rgb(255, 255, 255), 200)
          trafficSpawnedLimit = 5
        end if
      end if
    3:
      if (the ticks - pointsFlagStartTicks) > 200 then
        stageState = 4
        sprite(1).doPanel(23, rgb(255, 255, 255), 1)
        pointsFlagStartTicks = the ticks
      end if
    4:
      if (the ticks - pointsFlagStartTicks) > 300 then
        stageState = 0
      end if
    5:
      if (the ticks - pointsFlagStartTicks) > 400 then
        sprite(1).doPanel(26, rgb(255, 255, 255), 300)
        stageState = 0
      end if
    6:
      stageState = 0
      g_CurrentScore = 0
    7:
      if (the ticks - pointsFlagStartTicks) > 300 then
        stageState = 0
        g_CurrentScore = 0
      end if
  end case
  diffVector = mPointsFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
  if diffVector.magnitude < 120 then
    pushVector = diffVector.getNormalized() * (120 - diffVector.magnitude)
    velVector = velVector - (pushVector / g_FPSFactor)
    rotVector.z = rotVector.z - (pushVector.x * 0.40000000000000002 / g_FPSFactor)
    rotVector.x = rotVector.x + (pushVector.z * 0.40000000000000002 / g_FPSFactor)
    pushMag = pushVector.magnitude / g_FPSFactor
    if pushMag > 20 then
      g_DamagePercent = g_DamagePercent + 0.04
      if g_Muted = 0 then
        puppetSound(1, member(19, "Sfx"))
        soundMemberList[1] = member(19, "Sfx")
      end if
    else
      if pushMag > 15 then
        g_DamagePercent = g_DamagePercent + 0.03
        if g_Muted = 0 then
          puppetSound(1, member(18, "Sfx"))
          soundMemberList[1] = member(18, "Sfx")
        end if
      else
        if pushMag > 10 then
          g_DamagePercent = g_DamagePercent + 0.02
          if g_Muted = 0 then
            puppetSound(1, member(17, "Sfx"))
            soundMemberList[1] = member(17, "Sfx")
          end if
        else
          if pushMag > 5 then
            g_DamagePercent = g_DamagePercent + 0.01
            if g_Muted = 0 then
              puppetSound(1, member(16, "Sfx"))
              soundMemberList[1] = member(16, "Sfx")
            end if
          end if
        end if
      end if
    end if
  end if
end

on doSetupPointPods
  pointPodPointer = 1
  stagePodPosList = []
  tempList = [vector(33700.0, -5.55999999999999961, 32292.75999999999839929), vector(34248.51999999999679858, -5.55999999999999961, 33393.58999999999650754), vector(34249.87000000000261934, -5.54999999999999982, 34251.59999999999854481), vector(32425.54000000000087311, -5.55999999999999961, 34296.66000000000349246), vector(31055.54000000000087311, -5.53000000000000025, 34154.81999999999970896), vector(30965.61000000000058208, -5.54999999999999982, 32434.06999999999970896), vector(31253.95999999999912689, 90.32999999999999829, 30927.50999999999839929), vector(31400.0, 14.44999999999999929, 29937.36000000000058208), vector(32449.40000000000145519, -5.55999999999999961, 29433.90999999999985448), vector(34645.26000000000203727, 14.44999999999999929, 30284.81000000000130967), vector(34740.25, -5.61000000000000032, 32515.43999999999869033), vector(34727.76000000000203727, 14.51999999999999957, 34602.72000000000116415), vector(32443.77000000000043656, -6.53000000000000025, 34760.86000000000058208), vector(30326.0, 14.34999999999999964, 34699.69000000000232831), vector(30315.43000000000029104, -3.20999999999999996, 32529.9500000000007276), vector(30300.40000000000145519, 14.52999999999999936, 30271.18999999999869033), vector(32323.27999999999883585, -5.23000000000000043, 30121.63999999999941792), vector(33088.62999999999738066, -5.54999999999999982, 30838.5), vector(33891.94999999999708962, -5.54999999999999982, 30719.75), vector(34082.63999999999941792, -5.55999999999999961, 31721.7000000000007276)]
  stagePodPosList.add(tempList)
  tempList = [vector(23800.0, 14.4399999999999995, 29404.34999999999854481), vector(22687.0499999999992724, -1.69999999999999996, 29073.09999999999854481), vector(22601.29000000000087311, -5.59999999999999964, 27591.83000000000174623), vector(22800.0, 13.91999999999999993, 25744.58000000000174623), vector(25513.31000000000130967, 14.4399999999999995, 26352.4500000000007276), vector(27171.34000000000014552, 12.75999999999999979, 25249.24000000000160071), vector(28100.0, 13.9399999999999995, 25222.2000000000007276), vector(29527.18000000000029104, 14.44999999999999929, 26372.49000000000160071), vector(30430.18999999999869033, 14.44999999999999929, 26113.36000000000058208), vector(31476.88999999999941792, 14.4399999999999995, 26384.9500000000007276), vector(32484.83000000000174623, -5.25, 24907.15000000000145519), vector(34467.95999999999912689, 14.44999999999999929, 26375.34000000000014552), vector(35433.51000000000203727, 14.4399999999999995, 26129.72999999999956344), vector(38068.41999999999825377, 14.44999999999999929, 25946.56999999999970896), vector(39461.47000000000116415, 14.4399999999999995, 26377.72999999999956344), vector(40445.95999999999912689, 14.4399999999999995, 26128.81999999999970896), vector(41465.23999999999796273, 14.4399999999999995, 26368.61000000000058208), vector(42471.94999999999708962, 2.2200000000000002, 24940.13000000000101863), vector(43668.88999999999941792, 14.44999999999999929, 25193.25999999999839929), vector(45460.95999999999912689, 14.44999999999999929, 25159.74000000000160071), vector(46919.0, 14.48000000000000043, 25578.40000000000145519), vector(47523.48000000000320142, 508.41000000000002501, 27436.99000000000160071), vector(47500.0, -5.55999999999999961, 28883.84000000000014552), vector(46843.27999999999883585, 18.14000000000000057, 29320.50999999999839929), vector(45340.48000000000320142, 13.76999999999999957, 29675.84999999999854481), vector(43619.13999999999941792, 14.46000000000000085, 29484.93000000000029104), vector(42594.23000000000320142, -1.73999999999999999, 29278.61999999999898137), vector(40485.97000000000116415, 15.11999999999999922, 28646.20999999999912689), vector(39523.36000000000058208, 14.4399999999999995, 28867.75), vector(38463.93000000000029104, 14.4399999999999995, 28643.54000000000087311), vector(37537.80000000000291038, -6.01999999999999957, 29339.84000000000014552), vector(36445.16000000000349246, 14.66000000000000014, 29379.31999999999970896), vector(34578.44999999999708962, 14.44999999999999929, 29341.49000000000160071), vector(30314.34000000000014552, 15.24000000000000021, 29365.16999999999825377), vector(25110.29000000000087311, 14.49000000000000021, 29357.54000000000087311)]
  stagePodPosList.add(tempList)
  tempList = [vector(11884.12999999999919964, -5.55999999999999961, 3217.63000000000010914), vector(11909.22999999999956344, 144.49000000000000909, 1362.78999999999996362), vector(12498.70999999999912689, 144.44999999999998863, 467.79000000000002046), vector(13475.07999999999992724, 144.44999999999998863, 448.17000000000001592), vector(14525.68000000000029104, 144.44999999999998863, 1317.28999999999996362), vector(14520.7000000000007276, 295.11000000000001364, 2288.3000000000001819), vector(13478.21999999999934516, 294.44999999999998863, 2682.51000000000021828), vector(12154.62999999999919964, 294.44999999999998863, 2401.28000000000020009), vector(11888.59000000000014552, 444.31999999999999318, 1341.60999999999989996), vector(12496.84000000000014552, 444.43999999999999773, 463.51999999999998181), vector(13476.07999999999992724, 444.43999999999999773, 766.90999999999996817), vector(14485.53000000000065484, 445.87999999999999545, 1426.84999999999990905), vector(14545.79000000000087311, 592.04999999999995453, 2102.3000000000001819), vector(13484.02000000000043656, 594.44000000000005457, 2717.42000000000007276), vector(11992.5, 594.45000000000004547, 2181.73999999999978172), vector(11940.1000000000003638, 743.0, 1330.83999999999991815), vector(12725.18000000000029104, 744.45000000000004547, 616.17999999999994998), vector(14519.5499999999992724, 744.50999999999999091, 1320.16000000000008185), vector(14526.4500000000007276, 892.67999999999994998, 2310.61999999999989086), vector(13046.19000000000050932, 894.50999999999999091, 3004.96000000000003638), vector(10423.57999999999992724, 743.19000000000005457, 1300.78999999999996362), vector(10981.12000000000080036, 744.60000000000002274, 920.91999999999995907), vector(11553.61000000000058208, 594.88999999999998636, 2131.92000000000007276), vector(10900.5, 594.32000000000005002, 2633.13999999999987267), vector(10481.84000000000014552, 442.25999999999999091, 1306.79999999999995453), vector(10939.55999999999949068, 444.45999999999997954, 902.61000000000001364), vector(11558.27000000000043656, 444.47000000000002728, 1238.78999999999996362), vector(11526.45999999999912689, 294.76999999999998181, 2242.86999999999989086), vector(10467.67000000000007276, 145.11000000000001364, 1174.78999999999996362), vector(11278.40999999999985448, 144.38999999999998636, 1037.41000000000008185), vector(11565.22999999999956344, -5.33999999999999986, 2992.23999999999978172), vector(11545.3999999999996362, -5.55999999999999961, 3708.59000000000014552)]
  stagePodPosList.add(tempList)
  tempList = [vector(3381.90000000000009095, -5.58000000000000007, 3256.38000000000010914), vector(2600.78999999999996362, -5.57000000000000028, 4191.17000000000007276), vector(2486.34999999999990905, 195.0, 5949.93999999999959982), vector(2697.32000000000016371, 194.87000000000000455, 7620.1999999999998181), vector(3777.78999999999996362, -5.54999999999999982, 11827.04000000000087311), vector(3756.51999999999998181, 199.31000000000000227, 13548.59000000000014552), vector(3531.55999999999994543, 690.30999999999994543, 21408.11999999999898137), vector(3554.96999999999979991, 904.97000000000002728, 22506.63000000000101863), vector(3491.63000000000010914, 699.59000000000003183, 31429.54000000000087311), vector(3541.55999999999994543, 909.99000000000000909, 32523.74000000000160071), vector(3606.8000000000001819, 906.29999999999995453, 37517.94000000000232831), vector(2979.86999999999989086, 842.87999999999999545, 38042.98000000000320142), vector(1728.96000000000003638, 822.89999999999997726, 38192.86000000000058208), vector(996.35000000000002274, 890.47000000000002728, 37323.62000000000261934), vector(972.71000000000003638, 913.01999999999998181, 35800.0), vector(4566.43000000000029104, -5.55999999999999961, 35025.58999999999650754), vector(5482.32999999999992724, -5.62999999999999989, 35901.08999999999650754), vector(4545.72999999999956344, -5.58999999999999986, 36842.45999999999912689), vector(1133.25999999999999091, -5.08000000000000007, 36435.19999999999708962), vector(1287.38000000000010914, -5.15000000000000036, 38566.51000000000203727), vector(8576.78000000000065484, -5.49000000000000021, 40320.45999999999912689), vector(12687.48999999999978172, -5.55999999999999961, 39665.22000000000116415), vector(13253.45999999999912689, -5.55999999999999961, 36372.61000000000058208), vector(13272.37000000000080036, -5.54999999999999982, 31401.45999999999912689), vector(12866.47999999999956344, -5.54999999999999982, 30180.4500000000007276), vector(11776.55999999999949068, -5.54999999999999982, 29557.81000000000130967), vector(11168.22999999999956344, -5.62000000000000011, 28290.52999999999883585), vector(10996.05999999999949068, -5.53000000000000025, 23990.02999999999883585), vector(11295.12000000000080036, -5.54999999999999982, 21129.91999999999825377), vector(11316.55999999999949068, -5.55999999999999961, 17319.86999999999898137), vector(11081.68000000000029104, -5.99000000000000021, 15300.0), vector(11345.13999999999941792, -5.54999999999999982, 12314.48999999999978172), vector(11074.28000000000065484, -5.55999999999999961, 10352.45999999999912689), vector(11314.46999999999934516, -5.54999999999999982, 7454.21000000000003638), vector(11086.2000000000007276, -5.62000000000000011, 5318.06999999999970896), vector(9135.8999999999996362, -5.54999999999999982, 3446.48999999999978172), vector(5346.98999999999978172, -5.54999999999999982, 3532.63999999999987267)]
  stagePodPosList.add(tempList)
  tempList = [vector(30942.25999999999839929, 137.53999999999999204, 762.07000000000005002), vector(29115.4500000000007276, 141.53000000000000114, 1011.83000000000004093), vector(27716.86999999999898137, 111.93999999999999773, 1673.75999999999999091), vector(26173.5, 110.95000000000000284, 1179.25), vector(24056.86999999999898137, 125.73000000000000398, 318.79000000000002046), vector(20987.34999999999854481, 106.48999999999999488, 278.14999999999997726), vector(19266.33000000000174623, 124.59999999999999432, 1357.25), vector(17600.33000000000174623, 109.84000000000000341, 1999.07999999999992724), vector(15138.93000000000029104, 13.38000000000000078, 3417.84999999999990905), vector(16841.25999999999839929, 14.4399999999999995, 5637.84000000000014552), vector(17008.29000000000087311, 14.41999999999999993, 9707.53000000000065484), vector(16999.61999999999898137, 13.36999999999999922, 15000.31999999999970896), vector(18953.38999999999941792, 1101.27999999999997272, 18701.88000000000101863), vector(15882.6499999999996362, 103.54999999999999716, 18784.56999999999970896), vector(13815.32999999999992724, 14.07000000000000028, 15865.61000000000058208), vector(12346.95999999999912689, -5.17999999999999972, 14939.4500000000007276), vector(11401.62999999999919964, -5.49000000000000021, 11889.73999999999978172), vector(11748.2999999999992724, -5.54000000000000004, 10002.0), vector(16259.48999999999978172, 14.10999999999999943, 9681.11000000000058208), vector(18275.72000000000116415, 14.78999999999999915, 6682.28999999999996362), vector(20382.54000000000087311, 14.41999999999999993, 5510.77999999999974534), vector(29157.63999999999941792, 13.13000000000000078, 6410.77000000000043656), vector(31997.90000000000145519, 14.9399999999999995, 3944.92999999999983629), vector(35170.0, 14.30000000000000071, 2979.92000000000007276), vector(36956.01000000000203727, 14.47000000000000064, 2977.4499999999998181), vector(36323.05999999999767169, 14.58999999999999986, 6829.06999999999970896), vector(27583.7000000000007276, 336.99000000000000909, 7382.31000000000040018), vector(23905.83000000000174623, -5.5, 7266.73999999999978172), vector(22777.34000000000014552, 2.66999999999999993, 6857.27999999999974534), vector(19993.75999999999839929, -4.45000000000000018, 7356.47000000000025466), vector(17869.43000000000029104, 311.42000000000001592, 7394.48999999999978172), vector(15454.32999999999992724, 33.90999999999999659, 7393.57999999999992724), vector(14291.93000000000029104, 14.38000000000000078, 7963.96000000000003638), vector(14293.84000000000014552, 14.46000000000000085, 8923.3500000000003638), vector(14297.26000000000021828, 14.44999999999999929, 9896.18000000000029104), vector(15872.65999999999985448, 14.46000000000000085, 11999.61000000000058208), vector(18473.24000000000160071, 15.96000000000000085, 12022.53000000000065484), vector(22276.97999999999956344, -6.13999999999999968, 12490.19000000000050932), vector(25717.49000000000160071, -5.48000000000000043, 12536.69000000000050932), vector(27321.31000000000130967, -5.50999999999999979, 12494.67000000000007276), vector(27476.59000000000014552, -5.54000000000000004, 10079.09000000000014552), vector(27512.52000000000043656, -5.54999999999999982, 6774.93999999999959982), vector(27498.93999999999869033, -5.50999999999999979, 3488.1999999999998181), vector(29849.16999999999825377, 14.53999999999999915, 2099.21999999999979991), vector(32642.81000000000130967, 14.41999999999999993, 2618.9499999999998181), vector(34899.12999999999738066, 14.00999999999999979, 1646.36999999999989086), vector(33939.26999999999679858, 136.37999999999999545, 827.66999999999995907), vector(32415.97999999999956344, 109.98000000000000398, 940.19000000000005457)]
  stagePodPosList.add(tempList)
  tempList = [vector(27220.91999999999825377, -5.54999999999999982, 40423.36000000000058208), vector(23313.02000000000043656, 348.68999999999999773, 42181.70999999999912689), vector(20929.56000000000130967, 93.29999999999999716, 40529.73000000000320142), vector(15751.5499999999992724, -5.87999999999999989, 40732.08999999999650754), vector(14188.56999999999970896, 14.85999999999999943, 43695.08999999999650754), vector(12559.62000000000080036, 287.31000000000000227, 46843.33999999999650754), vector(13754.77000000000043656, -4.62999999999999989, 49560.11000000000058208), vector(15368.59000000000014552, 220.28000000000000114, 51647.19999999999708962), vector(15767.81999999999970896, 344.42000000000001592, 54087.72000000000116415), vector(17858.09999999999854481, 185.86000000000001364, 54795.95999999999912689), vector(23337.13999999999941792, 206.40999999999999659, 54570.61000000000058208), vector(30630.15000000000145519, 335.77999999999997272, 53862.51999999999679858), vector(37498.38999999999941792, 230.87999999999999545, 54536.19000000000232831), vector(46240.77999999999883585, 244.94999999999998863, 53983.15000000000145519), vector(48786.44999999999708962, -3.33999999999999986, 53971.88999999999941792), vector(50034.30000000000291038, -5.55999999999999961, 52511.41999999999825377), vector(52079.63999999999941792, -5.65000000000000036, 52535.5), vector(52468.55000000000291038, -5.55999999999999961, 47583.75), vector(49582.76000000000203727, 153.50999999999999091, 46966.54000000000087311), vector(49510.20999999999912689, 107.06999999999999318, 44740.69000000000232831), vector(48092.01000000000203727, 249.08000000000001251, 40269.11000000000058208), vector(45410.97000000000116415, 121.10999999999999943, 40532.45999999999912689), vector(40633.5, -6.37999999999999989, 41040.31999999999970896), vector(35457.41000000000349246, 67.92000000000000171, 40431.95999999999912689), vector(31547.13000000000101863, 7.70000000000000018, 40232.55999999999767169), vector(29269.27999999999883585, 203.02000000000001023, 40905.23000000000320142), vector(27626.61999999999898137, -5.48000000000000043, 39878.48000000000320142)]
  stagePodPosList.add(tempList)
  tempList = [vector(1128.44000000000005457, -5.55999999999999961, 36359.5), vector(1098.72000000000002728, -5.55999999999999961, 38761.75), vector(4215.86999999999989086, -5.55999999999999961, 38661.70999999999912689), vector(5249.77000000000043656, -5.55999999999999961, 33236.94000000000232831), vector(4303.36999999999989086, -5.55999999999999961, 31359.88999999999941792), vector(1122.40000000000009095, -5.53000000000000025, 31341.63000000000101863), vector(1101.84999999999990905, -5.55999999999999961, 33688.44999999999708962), vector(4133.8999999999996362, -5.55999999999999961, 33674.63999999999941792), vector(5456.18999999999959982, -5.55999999999999961, 22620.38999999999941792), vector(4267.67000000000007276, -5.54999999999999982, 21359.40000000000145519), vector(1101.23000000000001819, -5.53000000000000025, 21318.11000000000058208), vector(1101.53999999999996362, -5.54999999999999982, 23652.83000000000174623), vector(4095.01999999999998181, -5.55999999999999961, 23648.36000000000058208), vector(5517.68999999999959982, -5.55999999999999961, 16896.49000000000160071), vector(2468.65999999999985448, 194.44999999999998863, 15925.57999999999992724), vector(2533.15000000000009095, 194.28000000000000114, 17390.95999999999912689), vector(2502.90999999999985448, 189.74000000000000909, 19147.9500000000007276), vector(2472.30999999999994543, 729.13999999999998636, 21502.02000000000043656), vector(2508.25, 197.25, 27623.81999999999970896), vector(2514.48000000000001819, 191.5, 29161.22000000000116415), vector(2469.98000000000001819, 855.37000000000000455, 32058.13000000000101863), vector(2506.61000000000012733, 906.65999999999996817, 37630.94999999999708962), vector(2528.98999999999978172, 187.87000000000000455, 41005.43000000000029104), vector(2487.46000000000003638, 199.55000000000001137, 42492.62999999999738066), vector(2533.67000000000007276, 188.21999999999999886, 44046.36000000000058208), vector(2486.01999999999998181, 189.43999999999999773, 46122.12999999999738066), vector(2505.75, 200.66999999999998749, 47583.84999999999854481), vector(2506.36000000000012733, 200.24000000000000909, 48946.88999999999941792), vector(2509.63999999999987267, 194.53999999999999204, 50981.34999999999854481), vector(2491.80999999999994543, 194.08000000000001251, 52444.83000000000174623), vector(2542.19000000000005457, 194.08000000000001251, 53870.08999999999650754), vector(2629.05999999999994543, -5.55999999999999961, 56640.01999999999679858), vector(4445.34000000000014552, -5.54999999999999982, 56968.94000000000232831), vector(5036.57999999999992724, -5.54999999999999982, 49913.05999999999767169), vector(5265.5, -5.54999999999999982, 37861.09999999999854481), vector(2827.48999999999978172, -5.63999999999999968, 37357.34999999999854481)]
  stagePodPosList.add(tempList)
  tempList = [vector(12216.37999999999919964, -5.54999999999999982, 40354.83999999999650754), vector(13080.87999999999919964, -5.54000000000000004, 39148.52999999999883585), vector(13350.86000000000058208, -6.37999999999999989, 36439.70999999999912689), vector(13319.98999999999978172, -5.65000000000000036, 33970.83999999999650754), vector(12254.19000000000050932, -5.42999999999999972, 32418.97999999999956344), vector(8770.37999999999919964, -5.65000000000000036, 33520.87000000000261934), vector(7500.0, -5.65000000000000036, 63500.0)]
  stagePodPosList.add(tempList)
  mPointPod = w.model("PointsPod_001")
  mNextPointPod = mPointPod.cloneDeep("NextPointPodClone")
  mNextPointPod.shaderList[1].emissive = rgb(0, 0, 0)
  mNextPointPod.shaderList[2].emissive = rgb(0, 0, 0)
  mNextPointPod.shaderList[1].diffuse = rgb(0, 100, 0)
  mNextPointPod.shaderList[2].diffuse = rgb(0, 100, 0)
  mNextPointPod.shaderList[3].blend = 0
  mPointPod.worldPosition = vector(12500, 0, 4000)
  mPointPod.shaderList[3].texture.renderFormat = #rgba4444
  mPointPod.removeFromWorld()
  mNextPointPod.removeFromWorld()
end

on doPointPods
  case stageState of
    0:
      nothing()
    1:
      mPointPod.shaderList[3].textureTransform.translate(0, -0.05, 0)
    2:
      if (currentStage = 8) and (pointPodPointer = 6) then
        mNextPointPod.worldPosition = mPlane.worldPosition
      end if
      if (currentStage = 8) and (pointPodPointer = 7) then
        mPointPod.worldPosition = mPlane.worldPosition
      end if
      diffVector = gPlayerCar.worldPosition - mPointPod.worldPosition
      tempMag = diffVector.magnitude
      if tempMag < 200 then
        pointPodPointer = pointPodPointer + 1
        g_CurrentScore = g_CurrentScore + 100
        if g_Muted = 0 then
          puppetSound(1, member(29, "Sfx"))
          soundMemberList[1] = member(29, "Sfx")
        end if
        if pointPodPointer < stagePodPosList[currentStage].count then
          mPointPod.worldPosition = stagePodPosList[currentStage][pointPodPointer]
          mNextPointPod.worldPosition = stagePodPosList[currentStage][pointPodPointer + 1]
          tempPos = mNextPointPod.worldPosition
          tempPos.y = mPointPod.worldPosition.y
          mPointPod.pointAt(tempPos)
          tempPos = mPointPod.worldPosition
          tempPos.y = mNextPointPod.worldPosition.y
          mNextPointPod.pointAt(tempPos)
        else
          if pointPodPointer = stagePodPosList[currentStage].count then
            tempPos = mPointPod.worldPosition
            mPointPod.worldPosition = stagePodPosList[currentStage][pointPodPointer]
            tempPos.y = mPointPod.worldPosition.y
            mPointPod.pointAt(tempPos)
            mPointPod.rotate(0, 180, 0)
            mNextPointPod.removeFromWorld()
          else
            mPointPod.removeFromWorld()
          end if
        end if
      end if
      mPointPod.shaderList[3].textureTransform.translate(0.025, -0.05, 0)
  end case
end

on doSetupRadar
  tempTexture = w.newTexture("RadarRingOverlayTexture", #fromCastMember, member(54, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarRing = cam.overlay.count
  tempTexture = w.newTexture("RadarFixOverlayTexture", #fromCastMember, member(55, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarFix = cam.overlay.count
  tempTexture = w.newTexture("RadarRaceOverlayTexture", #fromCastMember, member(56, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarRace = cam.overlay.count
  tempTexture = w.newTexture("RadarStageOverlayTexture", #fromCastMember, member(57, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarStage = cam.overlay.count
  tempTexture = w.newTexture("RadarPointPodOverlayTexture", #fromCastMember, member(58, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarPointPod = cam.overlay.count
  tempTexture = w.newTexture("RadarRacePodOverlayTexture", #fromCastMember, member(59, "Textures"))
  tempTexture.renderFormat = #rgba8888
  cam.addOverlay(tempTexture, point(20, 400), 0)
  oRadarRacePod = cam.overlay.count
end

on doRadar
  tempFrontVector = -cam.getWorldTransform().zAxis
  tempSideVector = -cam.getWorldTransform().xAxis
  tempPointPodParent = mPointPod.parent
  tempRacePodParent = mRacePod.parent
  if (tempPointPodParent <> w.group("World")) and (tempRacePodParent <> w.group("World")) then
    tempToFixVector = cam.worldPosition - mRixGarageDoor.worldPosition
    tempZDiff = tempFrontVector.dot(tempToFixVector)
    tempXDiff = tempSideVector.dot(tempToFixVector)
    tempVector = vector(tempXDiff, 0, tempZDiff)
    tempMag = tempVector.magnitude * 0.005
    tempVector.normalize()
    if tempMag > 32 then
      tempMag = 32
    end if
    tempPoint = point(44, 424) + point(tempVector.x * tempMag, tempVector.z * tempMag)
    cam.overlay[oRadarFix].loc = tempPoint
  else
    cam.overlay[oRadarFix].loc = point(-1000, -1000)
  end if
  if (tempPointPodParent <> w.group("World")) and (tempRacePodParent <> w.group("World")) then
    tempRace = 0
    repeat with i = g_CarsOwnedList.count down to 2
      if g_CarsOwnedList[i] = 0 then
        tempRace = i - 1
      end if
    end repeat
    if tempRace = 0 then
      tempRacePos = mRaceFlag.worldPosition
    else
      tempRacePos = raceFlagPosList[tempRace]
    end if
    tempToRaceVector = cam.worldPosition - tempRacePos
    tempZDiff = tempFrontVector.dot(tempToRaceVector)
    tempXDiff = tempSideVector.dot(tempToRaceVector)
    tempVector = vector(tempXDiff, 0, tempZDiff)
    tempMag = tempVector.magnitude * 0.005
    tempVector.normalize()
    if tempMag > 32 then
      tempMag = 32
    end if
    tempPoint = point(44, 424) + point(tempVector.x * tempMag, tempVector.z * tempMag)
    cam.overlay[oRadarRace].loc = tempPoint
  else
    cam.overlay[oRadarRace].loc = point(-1000, -1000)
  end if
  if (tempPointPodParent <> w.group("World")) and (tempRacePodParent <> w.group("World")) then
    tempToStageVector = cam.worldPosition - mPointsFlag.worldPosition
    tempZDiff = tempFrontVector.dot(tempToStageVector)
    tempXDiff = tempSideVector.dot(tempToStageVector)
    tempVector = vector(tempXDiff, 0, tempZDiff)
    tempMag = tempVector.magnitude * 0.005
    tempVector.normalize()
    if tempMag > 32 then
      tempMag = 32
    end if
    tempPoint = point(44, 424) + point(tempVector.x * tempMag, tempVector.z * tempMag)
    cam.overlay[oRadarStage].loc = tempPoint
  else
    cam.overlay[oRadarStage].loc = point(-1000, -1000)
  end if
  if tempPointPodParent = w.group("World") then
    tempToStageVector = cam.worldPosition - mPointPod.worldPosition
    tempZDiff = tempFrontVector.dot(tempToStageVector)
    tempXDiff = tempSideVector.dot(tempToStageVector)
    tempVector = vector(tempXDiff, 0, tempZDiff)
    tempMag = tempVector.magnitude * 0.01
    tempVector.normalize()
    if tempMag > 32 then
      tempMag = 32
    end if
    tempPoint = point(48, 428) + point(tempVector.x * tempMag, tempVector.z * tempMag)
    cam.overlay[oRadarPointPod].loc = tempPoint
  else
    cam.overlay[oRadarPointPod].loc = point(-1000, -1000)
  end if
  if tempRacePodParent = w.group("World") then
    tempToStageVector = cam.worldPosition - mRacePod.worldPosition
    tempZDiff = tempFrontVector.dot(tempToStageVector)
    tempXDiff = tempSideVector.dot(tempToStageVector)
    tempVector = vector(tempXDiff, 0, tempZDiff)
    tempMag = tempVector.magnitude * 0.01
    tempVector.normalize()
    if tempMag > 32 then
      tempMag = 32
    end if
    tempPoint = point(48, 428) + point(tempVector.x * tempMag, tempVector.z * tempMag)
    cam.overlay[oRadarRacePod].loc = tempPoint
  else
    cam.overlay[oRadarRacePod].loc = point(-1000, -1000)
  end if
end

on doSetupRaceFlag
  raceState = 0
  opponentSpawned = 0
  currentRace = 1
  prevCurrentRace = 0
  raceFlagStartTicks = 0
  raceLightPhase = 0
  raceLightStartTicks = 0
  mRaceFlag = w.model("RaceFlag_001")
  mRaceFlag.worldPosition = vector(0, -100000, 0)
  raceFlagPosList = [vector(32501.52999999999883585, -2.68999999999999995, 24293.58000000000174623), vector(50049.05999999999767169, 14.4399999999999995, 29431.88999999999941792), vector(52984.54000000000087311, 14.47000000000000064, 38827.75), vector(31150.02999999999883585, -5.55999999999999961, 42488.19999999999708962), vector(19312.33000000000174623, 14.4399999999999995, 29473.47999999999956344)]
  raceFlagRotList = [vector(0, 0, 0), vector(0, 270, 0), vector(0, 180, 0), vector(0, 90, 0), vector(0, 90, 0)]
  tRaceFlagArrows = mRaceFlag.shaderList[4]
  tRaceFlagLights = mRaceFlag.shaderList[3]
end

on doRaceFlag
  case raceLightPhase of
    0:
      if tRaceFlagLights.textureTransform.position.x > -0.0 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    1:
      if tRaceFlagLights.textureTransform.position.x > -0.25 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    2:
      if tRaceFlagLights.textureTransform.position.x > -0.5 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
    3:
      if tRaceFlagLights.textureTransform.position.x > -0.75 then
        tRaceFlagLights.textureTransform.position.x = tRaceFlagLights.textureTransform.position.x - 0.05
      end if
  end case
  case raceState of
    0:
      currentRace = 7
      tempMinMag = 100000000000000000.0
      repeat with i = 1 to raceFlagPosList.count
        tempDiff = gPlayerCar.worldPosition - raceFlagPosList[i]
        tempMag = tempDiff.magnitude
        if tempMag < tempMinMag then
          currentRace = i
          tempMinMag = tempMag
        end if
      end repeat
      if currentRace <> prevCurrentRace then
        put prevCurrentRace && "NEW Current Race " & currentRace
        mRaceFlag.worldPosition = raceFlagPosList[currentRace]
        mRaceFlag.transform.rotation = raceFlagRotList[currentRace]
        opponentSpawned = 0
      end if
      if opponentSpawned = 0 then
        tempDiff = gPlayerCar.worldPosition - mRaceFlag.worldPosition
        if tempDiff.magnitude < 5000 then
          opponentSpawned = 1
          if gOpponentCar.child.count > 0 then
            mOpponentCar.removeFromWorld()
          end if
          gOpponentCar.worldPosition = mRaceFlag.worldPosition + vector(0, 5, 0) + (mRaceFlag.transform.zAxis * 140) - (mRaceFlag.transform.xAxis * 140)
          mOpponentCar = mOpponentShellList[currentRace]
          gOpponentCar.addChild(mOpponentCar)
          mOpponentCar.transform.position = vector(0, 0, 0)
          mOpponentCar.transform.rotation = raceFlagRotList[currentRace] + vector(0, 180, 0)
          mOpponentCar.addChild(gOpponentCage)
          gOpponentCage.transform.rotation = vector(0, 0, 0)
          gOpponentCage.transform.position = vector(0, 0, 0)
          mOpponentTireFL = mOpponentCar.child[4]
          mOpponentTireBL = mOpponentCar.child[5]
          mOpponentTireFR = mOpponentCar.child[6].child[2]
          mOpponentTireBR = mOpponentCar.child[6].child[3]
        end if
      end if
      if stageState > 0 then
        tRaceFlagArrows.textureTransform.position = vector(0, 0, 0)
      else
        tRaceFlagArrows.textureTransform.translate(0, -0.02, 0)
      end if
      diffVector = mRaceFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
      if (diffVector.magnitude < 600) and (stageState = 0) then
        tempXAxis = -mRaceFlag.transform.xAxis
        tempZAxis = -mRaceFlag.transform.zAxis
        tempCarZAxis = mPlayerCar.getWorldTransform().zAxis
        tempXDist = tempXAxis.dot(diffVector)
        tempZDist = tempZAxis.dot(diffVector)
        tempForward = tempZAxis.dot(tempCarZAxis)
        tempForwardVel = tempZAxis.dot(velVector)
        prevDiffVector = mRaceFlag.worldPosition - gPlayerCar.worldPosition
        tempPrevZDist = tempZAxis.dot(prevDiffVector)
        if (tempXDist > 0) and (tempXDist < 250) and (tempZDist > 0) and (tempZDist < 400) and (tempForward > 0.75) then
          if (tempZDist < 100) and (tempForwardVel > 0) and (tempPrevZDist >= 100) then
            pushVector = -tempZAxis * (100 - tempZDist)
            pushVector = pushVector - (tempXAxis * (160 - tempXDist) * 0.10000000000000001)
            racePodPointer = 1
            velVector = velVector + (pushVector / g_FPSFactor)
            sprite(1).doPanel(18, rgb(255, 255, 255), 0)
            sprite(1).timeList = [0, 0, 0, 0, 0]
            g_TimerState = 0
            sprite(1).updateTimer()
            g_CurrentScore = 0
            raceState = 1
            if g_Muted = 0 then
              puppetSound(1, member(26, "Sfx"))
              soundMemberList[1] = member(26, "Sfx")
            end if
            raceLightStartTicks = the ticks
            raceLightPhase = 1
            tRaceFlagLights.textureTransform.position.x = 0.0
            tRaceFlagArrows.textureTransform.position = vector(0, 0, 0)
            mRacePod.addToWorld()
            mRacePod.worldPosition = racePodPosList[currentRace][racePodPointer]
            mNextRacePod.addToWorld()
            mNextRacePod.worldPosition = racePodPosList[currentRace][racePodPointer + 1]
            tempPos = mNextRacePod.worldPosition
            tempPos.y = mRacePod.worldPosition.y
            mRacePod.pointAt(tempPos)
            tempPos = mRacePod.worldPosition
            tempPos.y = mNextRacePod.worldPosition.y
            mNextRacePod.pointAt(tempPos)
            repeat with i = 1 to mIgNitroList.count
              if i <= igNitroRacePosList[currentRace].count then
                mIgNitroList[i].visibility = #front
                mIgNitroList[i].worldPosition = igNitroRacePosList[currentRace][i] + vector(0, 55, 0)
                next repeat
              end if
              mIgNitroList[i].visibility = #none
            end repeat
          end if
        else
        end if
      end if
    1:
      diffVector = mRaceFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
      if diffVector.magnitude < 600 then
        tempXAxis = -mRaceFlag.transform.xAxis
        tempZAxis = -mRaceFlag.transform.zAxis
        tempCarZAxis = mPlayerCar.getWorldTransform().zAxis
        tempXDist = tempXAxis.dot(diffVector)
        tempZDist = tempZAxis.dot(diffVector)
        tempForward = tempZAxis.dot(tempCarZAxis)
        tempForwardVel = tempZAxis.dot(velVector)
        tempTwist = tempXAxis.dot(tempCarZAxis)
        if (tempXDist > 0) and (tempXDist < 250) and (tempZDist > 0) and (tempZDist < 400) and (tempForward > 0.75) then
          if (tempZDist < 100) and (tempForwardVel > 0) then
            pushVector = -tempZAxis * (100 - tempZDist)
            velVector = velVector + (pushVector / g_FPSFactor)
          end if
          rotVector.y = rotVector.y - (2 * tempTwist)
        else
          sprite(1).doPanel(28, rgb(255, 255, 255), 200)
          raceState = 0
          raceLightPhase = 0
          mRacePod.removeFromWorld()
          mNextRacePod.removeFromWorld()
          repeat with i = 1 to mIgNitroList.count
            mIgNitroList[i].visibility = #none
          end repeat
        end if
        if (the ticks - raceLightStartTicks) > 60 then
          raceLightStartTicks = the ticks
          raceLightPhase = raceLightPhase + 1
          sprite(1).doPanel(17 + raceLightPhase, rgb(255, 255, 255), 0)
          if raceLightPhase >= 3 then
            opponentState = 1
            opponentTargetPos = redDotPosList[currentRace][1]
            g_TimerState = -2
            racePodPointer = 1
            raceState = 2
            sprite(1).doPanel(20, rgb(255, 255, 255), 100)
          end if
        end if
      end if
    2:
      if racePodPointer > racePodPosList[currentRace].count then
        raceState = 3
        g_NumberRacesWon = g_NumberRacesWon + 1
        if g_DamagePercent <= 0.20000000000000001 then
          g_RacesNoDamage = g_RacesNoDamage + 1
        end if
        opponentDotID = 0
        g_TimerState = 0
        raceFlagStartTicks = the ticks
        raceLightPhase = 0
        if g_CarsOwnedList[currentRace + 1] = 0 then
          sprite(1).doPanel(34, rgb(255, 255, 255), 0)
          g_CarsOwnedList[currentRace + 1] = 1
          doSaveGameData()
        else
          sprite(1).doPanel(44, rgb(255, 255, 255), 0)
        end if
        if g_Muted = 0 then
          puppetSound(1, member(34, "Sfx"))
          soundMemberList[1] = member(27, "Sfx")
        end if
      else
        if opponentDotID > redDotPosList[currentRace].count then
          g_TimerState = 0
          opponentDotID = 0
          raceFlagStartTicks = the ticks
          raceLightPhase = 0
          if playerCarID > 1 then
            sprite(1).doPanel(37, rgb(255, 255, 255), 0)
          else
            sprite(1).doPanel(39, rgb(255, 255, 255), 0)
          end if
          raceState = 6
          if g_Muted = 0 then
            puppetSound(1, member(28, "Sfx"))
            soundMemberList[1] = member(28, "Sfx")
          end if
        end if
      end if
    3:
      if (the ticks - raceFlagStartTicks) > 300 then
        raceState = 4
        sprite(1).doPanel(35, rgb(255, 255, 255), 1)
        raceFlagStartTicks = the ticks
        sprite(1).fadeToBlackBlend = 100
        ferrisState = 0
        doResetFerrisWheel()
        if playerCarID > 1 then
          put "g_CarsOwnedList = " & g_CarsOwnedList
          tempID = 1
          raceFlagStartTicks = the ticks
          sprite(1).fadeToBlackBlend = 100
          g_DamagePercent = 0.0
          g_BoostPercent = 1.0
          ferrisState = 0
          doResetFerrisWheel()
          mPlayerCar = mShellList[playerCarID]
          gPlayerCar.addChild(mPlayerCar)
          mPlayerCar.transform.position = vector(0, 0, 0)
          mPlayerCar.addChild(gPlayerCage)
          gPlayerCage.transform.position = vector(0, 0, 0)
          gPlayerCage.transform.rotation = vector(0, 0, 0)
          mPlayerCar.addChild(mIgNitroBlastL)
          mPlayerCar.addChild(mIgNitroBlastR)
          mPlayerCar.addChild(gIgNitro)
          gIgNitro.addChild(mIgNitro)
          mIgNitroBlastL.transform.rotation = vector(10, 0, 0)
          mIgNitroBlastR.transform.rotation = vector(10, 0, 0)
          case playerCarID of
            1:
              mIgNitroBlastL.transform.position = vector(30, 20, -95)
              mIgNitroBlastL.transform.position = vector(30, 20, -95)
              gIgNitro.transform.position = vector(0, 85, -80)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            2:
              mIgNitroBlastL.transform.position = vector(-30, 25, -125)
              mIgNitroBlastR.transform.position = vector(30, 25, -125)
              gIgNitro.transform.position = vector(0, 96, -110)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            3:
              mIgNitroBlastL.transform.position = vector(-38, 25, -125)
              mIgNitroBlastR.transform.position = vector(38, 25, -125)
              gIgNitro.transform.position = vector(0, 92, -110)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            4:
              mIgNitroBlastL.transform.position = vector(-30, 22, -120)
              mIgNitroBlastR.transform.position = vector(30, 22, -120)
              gIgNitro.transform.position = vector(0, 94, -95)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            5:
              mIgNitroBlastL.transform.position = vector(-36, 20, -140)
              mIgNitroBlastR.transform.position = vector(36, 20, -140)
              gIgNitro.transform.position = vector(0, 92, -120)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            6:
              mIgNitroBlastL.transform.position = vector(-45, 25, -120)
              mIgNitroBlastR.transform.position = vector(45, 25, -120)
              gIgNitro.transform.position = vector(0, 105, -70)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
          end case
          mBonnet = mPlayerCar.child[1]
          mBoot = mPlayerCar.child[2]
          mDoorL = mPlayerCar.child[3]
          mTireFL = mPlayerCar.child[4]
          mTireBL = mPlayerCar.child[5]
          mDoorR = mPlayerCar.child[6].child[1]
          mTireFR = mPlayerCar.child[6].child[2]
          mTireBR = mPlayerCar.child[6].child[3]
        end if
        gPlayerCar.worldPosition = vector(32053.56999999999970896, -5.55999999999999961, 30797.50999999999839929)
        mPlayerCar.transform.rotation = vector(0, 45, 0)
        camAim = gPlayerCar.worldPosition + vector(-80, 50, 200)
        cam.worldPosition = gPlayerCar.worldPosition + vector(-80, 50, -200)
        mBoot.transform.rotation = vector(0, 0, 0)
        mBonnet.transform.rotation = vector(0, 0, 0)
        mDoorL.transform.rotation = vector(0, 0, 0)
        mDoorR.transform.rotation = vector(0, 0, 0)
        partRotList = [0, 0, 0, 0, 0, 0, 0]
        partRotVelList = [0, 0, 0, 0, 0, 0, 0]
        velVector = vector(0, 0, 0)
        rotVector = vector(0, 0, 0)
        opponentState = 0
        mOpponentCar.removeFromWorld()
        mRacePod.removeFromWorld()
        mNextRacePod.removeFromWorld()
        repeat with i = 1 to mIgNitroList.count
          mIgNitroList[i].visibility = #none
        end repeat
      end if
    4:
      if (the ticks - raceFlagStartTicks) > 300 then
        put "to 0 raceState"
        raceState = 0
      end if
      currentState = 0
    5:
      if (the ticks - raceFlagStartTicks) > 400 then
        sprite(1).doPanel(26, rgb(255, 255, 255), 300)
        raceState = 0
      end if
    6:
      if (the ticks - raceFlagStartTicks) > 400 then
        raceState = 0
        sprite(1).doPanel(38, rgb(255, 255, 255), 300)
        raceFlagStartTicks = the ticks
        sprite(1).fadeToBlackBlend = 100
        g_DamagePercent = 0.0
        g_BoostPercent = 1.0
        ferrisState = 0
        doResetFerrisWheel()
        if playerCarID > 1 then
          g_CarsOwnedList[playerCarID] = 0
          doSaveGameData()
          put "g_CarsOwnedList = " & g_CarsOwnedList
          tempID = 1
          repeat with i = 1 to 6
            if g_CarsOwnedList[i] > 0 then
              tempID = i
            end if
          end repeat
          g_CarsOwnedList[tempID] = 2
          playerCarID = tempID
          sprite(1).doPanel(38, rgb(255, 255, 255), 300)
          raceFlagStartTicks = the ticks
          sprite(1).fadeToBlackBlend = 100
          g_DamagePercent = 0.0
          g_BoostPercent = 1.0
          ferrisState = 0
          mBoot.transform.rotation = vector(0, 0, 0)
          mBonnet.transform.rotation = vector(0, 0, 0)
          mDoorL.transform.rotation = vector(0, 0, 0)
          mDoorR.transform.rotation = vector(0, 0, 0)
          partRotList = [0, 0, 0, 0, 0, 0, 0]
          partRotVelList = [0, 0, 0, 0, 0, 0, 0]
          doResetFerrisWheel()
          mPlayerCar = mShellList[playerCarID]
          gPlayerCar.addChild(mPlayerCar)
          mPlayerCar.transform.position = vector(0, 0, 0)
          mPlayerCar.addChild(gPlayerCage)
          gPlayerCage.transform.position = vector(0, 0, 0)
          gPlayerCage.transform.rotation = vector(0, 0, 0)
          mPlayerCar.addChild(mIgNitroBlastL)
          mPlayerCar.addChild(mIgNitroBlastR)
          mPlayerCar.addChild(gIgNitro)
          gIgNitro.addChild(mIgNitro)
          mIgNitroBlastL.transform.rotation = vector(10, 0, 0)
          mIgNitroBlastR.transform.rotation = vector(10, 0, 0)
          case playerCarID of
            1:
              mIgNitroBlastL.transform.position = vector(30, 20, -95)
              mIgNitroBlastL.transform.position = vector(30, 20, -95)
              gIgNitro.transform.position = vector(0, 85, -80)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            2:
              mIgNitroBlastL.transform.position = vector(-30, 25, -125)
              mIgNitroBlastR.transform.position = vector(30, 25, -125)
              gIgNitro.transform.position = vector(0, 96, -110)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            3:
              mIgNitroBlastL.transform.position = vector(-38, 25, -125)
              mIgNitroBlastR.transform.position = vector(38, 25, -125)
              gIgNitro.transform.position = vector(0, 92, -110)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            4:
              mIgNitroBlastL.transform.position = vector(-30, 22, -120)
              mIgNitroBlastR.transform.position = vector(30, 22, -120)
              gIgNitro.transform.position = vector(0, 94, -95)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            5:
              mIgNitroBlastL.transform.position = vector(-36, 20, -140)
              mIgNitroBlastR.transform.position = vector(36, 20, -140)
              gIgNitro.transform.position = vector(0, 92, -120)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
            6:
              mIgNitroBlastL.transform.position = vector(-45, 25, -120)
              mIgNitroBlastR.transform.position = vector(45, 25, -120)
              gIgNitro.transform.position = vector(0, 105, -70)
              gIgNitro.transform.rotation = vector(0, 0, 0)
              mIgNitro.transform.position = vector(0, -50, 0)
              mIgNitro.transform.rotation = vector(0, 0, 0)
          end case
          mBonnet = mPlayerCar.child[1]
          mBoot = mPlayerCar.child[2]
          mDoorL = mPlayerCar.child[3]
          mTireFL = mPlayerCar.child[4]
          mTireBL = mPlayerCar.child[5]
          mDoorR = mPlayerCar.child[6].child[1]
          mTireFR = mPlayerCar.child[6].child[2]
          mTireBR = mPlayerCar.child[6].child[3]
        end if
        gPlayerCar.worldPosition = vector(32053.56999999999970896, -5.55999999999999961, 30797.50999999999839929)
        mPlayerCar.transform.rotation = vector(0, 45, 0)
        camAim = gPlayerCar.worldPosition + vector(-80, 50, 200)
        cam.worldPosition = gPlayerCar.worldPosition + vector(-80, 50, -200)
        mBoot.transform.rotation = vector(0, 0, 0)
        mBonnet.transform.rotation = vector(0, 0, 0)
        mDoorL.transform.rotation = vector(0, 0, 0)
        mDoorR.transform.rotation = vector(0, 0, 0)
        partRotList = [0, 0, 0, 0, 0, 0, 0]
        partRotVelList = [0, 0, 0, 0, 0, 0, 0]
        velVector = vector(0, 0, 0)
        rotVector = vector(0, 0, 0)
        currentRace = 0
        opponentState = 0
        opponentDotID = 0
        mOpponentCar.removeFromWorld()
        gOpponentCar.transform.position = vector(-1000, -1000, -1000)
        mRacePod.removeFromWorld()
        mNextRacePod.removeFromWorld()
        repeat with i = 1 to mIgNitroList.count
          mIgNitroList[i].visibility = #none
        end repeat
      end if
    7:
      if (the ticks - raceFlagStartTicks) > 300 then
        raceState = 0
        opponentSpawned = 0
        opponentDotID = 0
        currentRace = 0
        opponentState = 0
        mOpponentCar.removeFromWorld()
        mRacePod.removeFromWorld()
        mNextRacePod.removeFromWorld()
        repeat with i = 1 to mIgNitroList.count
          mIgNitroList[i].visibility = #none
        end repeat
      end if
  end case
  diffVector = mRaceFlag.worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
  if diffVector.magnitude < 120 then
    pushVector = diffVector.getNormalized() * (120 - diffVector.magnitude)
    velVector = velVector - (pushVector / g_FPSFactor)
    rotVector.z = rotVector.z - (pushVector.x * 0.40000000000000002 / g_FPSFactor)
    rotVector.x = rotVector.x + (pushVector.z * 0.40000000000000002 / g_FPSFactor)
    pushMag = pushVector.magnitude / g_FPSFactor
    if pushMag > 20 then
      g_DamagePercent = g_DamagePercent + 0.04
      if g_Muted = 0 then
        puppetSound(1, member(19, "Sfx"))
        soundMemberList[1] = member(19, "Sfx")
      end if
    else
      if pushMag > 15 then
        g_DamagePercent = g_DamagePercent + 0.03
        if g_Muted = 0 then
          puppetSound(1, member(18, "Sfx"))
          soundMemberList[1] = member(18, "Sfx")
        end if
      else
        if pushMag > 10 then
          g_DamagePercent = g_DamagePercent + 0.02
          if g_Muted = 0 then
            puppetSound(1, member(17, "Sfx"))
            soundMemberList[1] = member(17, "Sfx")
          end if
        else
          if pushMag > 5 then
            g_DamagePercent = g_DamagePercent + 0.01
            if g_Muted = 0 then
              puppetSound(1, member(16, "Sfx"))
              soundMemberList[1] = member(16, "Sfx")
            end if
          end if
        end if
      end if
    end if
  end if
end

on doSetupRacePods
  racePodPointer = 1
  mRacePod = w.model("RacePod_001")
  mNextRacePod = mRacePod.cloneDeep("NextRacePodClone")
  mNextRacePod.shaderList[1].emissive = rgb(0, 0, 0)
  mNextRacePod.shaderList[2].emissive = rgb(0, 0, 0)
  mNextRacePod.shaderList[1].diffuse = rgb(50, 0, 50)
  mNextRacePod.shaderList[2].diffuse = rgb(50, 0, 50)
  mNextRacePod.shaderList[3].blend = 0
  mRacePod.worldPosition = vector(12500, 0, 4000)
  mRacePod.shaderList[3].texture.renderFormat = #rgba4444
  mRacePod.removeFromWorld()
  mNextRacePod.removeFromWorld()
end

on doRacePods
  case raceState of
    0:
      nothing()
    1, 2, 3, 4, 5, 7, 8:
      diffVector = gPlayerCar.worldPosition - mRacePod.worldPosition
      tempMag = diffVector.magnitude
      if tempMag < 800 then
        racePodPointer = racePodPointer + 1
        if racePodPointer < racePodPosList[currentRace].count then
          if g_Muted = 0 then
            puppetSound(1, member(29, "Sfx"))
            soundMemberList[1] = member(29, "Sfx")
          end if
          mRacePod.worldPosition = racePodPosList[currentRace][racePodPointer]
          mNextRacePod.worldPosition = racePodPosList[currentRace][racePodPointer + 1]
          tempPos = mNextRacePod.worldPosition
          tempPos.y = mRacePod.worldPosition.y
          mRacePod.pointAt(tempPos)
          tempPos = mRacePod.worldPosition
          tempPos.y = mRacePod.worldPosition.y
          mNextRacePod.pointAt(tempPos)
        else
          if racePodPointer = racePodPosList[currentRace].count then
            if g_Muted = 0 then
              puppetSound(1, member(29, "Sfx"))
              soundMemberList[1] = member(29, "Sfx")
            end if
            tempPos = mPointPod.worldPosition
            mRacePod.worldPosition = racePodPosList[currentRace][racePodPointer]
            tempPos.y = mRacePod.worldPosition.y
            mRacePod.pointAt(tempPos)
            mRacePod.rotate(0, 180, 0)
            mNextRacePod.removeFromWorld()
          else
            mRacePod.removeFromWorld()
          end if
        end if
      end if
      mRacePod.shaderList[3].textureTransform.translate(0.025, -0.05, 0)
    6:
      mRacePod.removeFromWorld()
      mNextRacePod.removeFromWorld()
  end case
end

on doSaveGameData
  set the floatPrecision to 2
  g_CarsDamageList[playerCarID] = g_DamagePercent
  g_CarsIgNitroList[playerCarID] = g_BoostPercent
  tempSaveString = "[" & g_CarsOwnedList & "," & g_CarsColorIDList & "," & g_CarsDamageList & "," & g_CarsIgNitroList & "]"
  put "SAVE GAME DATA = " & tempSaveString
  setPref(g_SaveGameName, tempSaveString)
end

on doSetupigNitro
  firstIgNitro = 1
  igNitroRot = 0
  mIgNitro = w.model("IgNitroBottle_001")
  mIgNitroList = []
  repeat with i = 1 to 10
    tempClone = mIgNitro.clone("IgNitroClone" && i)
    mIgNitroList.add(tempClone)
    mIgNitroList[i].visibility = #none
    mIgNitroList[i].transform.rotation = vector(0, 180, 120)
    mIgNitroList[i].transform.scale = vector(2, 2, 2)
  end repeat
  mIgNitroList[1].visibility = #front
  mIgNitroList[1].addToWorld()
  mIgNitroList[1].worldPosition = vector(34017.54000000000087311, 50, 32709.34000000000014552)
  igNitroRacePosList = []
  igNitroRacePosList.add([vector(42600.58000000000174623, -5.48000000000000043, 12511.96999999999934516), vector(50070.05000000000291038, 14.60999999999999943, 29341.56999999999970896)])
  igNitroRacePosList.add([vector(13105.4500000000007276, -5.55999999999999961, 43014.47000000000116415), vector(14760.87000000000080036, 14.19999999999999929, 43129.26000000000203727)])
  igNitroRacePosList.add([vector(47507.20999999999912689, -5.54999999999999982, 11486.69000000000050932), vector(40673.16000000000349246, 14.4399999999999995, 29494.2999999999992724)])
  igNitroRacePosList.add([vector(38064.51999999999679858, -5.48000000000000043, 52491.05999999999767169)])
  igNitroRacePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792), vector(2624.11000000000012733, -5.55999999999999961, 33092.94999999999708962)])
  igNitroStagePosList = []
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  igNitroStagePosList.add([vector(32052.29000000000087311, 75.18000000000000682, 51194.55000000000291038), vector(42533.06999999999970896, 632.62000000000000455, 53121.94999999999708962), vector(43737.09999999999854481, 20.01999999999999957, 43158.13999999999941792)])
  gIgNitro = w.newGroup("igNitroGroup")
  gIgNitro.addChild(mIgNitro)
end

on doIgNitro
  tempi = mIgNitroList.count
  repeat with i = 1 to tempi
    if mIgNitroList[i].visibility = #front then
      tempDiff = gPlayerCar.worldPosition - mIgNitroList[i].worldPosition
      tempMag = tempDiff.magnitude
      if tempMag < 150 then
        mIgNitroList[i].visibility = #none
        if firstIgNitro = 1 then
          firstIgNitro = 0
          sprite(1).doPanel(16, rgb(255, 255, 255), 300)
        end if
        if g_Muted = 0 then
          puppetSound(1, member(25, "Sfx"))
          soundMemberList[1] = member(25, "Sfx")
        end if
        igNitroRot = 250
        g_BoostPercent = 1
      end if
    end if
  end repeat
  gIgNitro.transform.rotation = vector(igNitroRot, 0, 0)
  igNitroRot = igNitroRot * 0.90000000000000002
  if igNitroRot < 0.10000000000000001 then
    igNitroRot = 0
  end if
end

on doSetupColorTank
  colorTankState = 0
  colorSplashPercent = 0
  colorSplashID = 1
  colorClacAngle = 0
  colorRot = 0
  colorPrevRot = 0
  colorTargetRot = 0
  mColorCans = w.model("FerrisColorPaintCans_000")
  mColorCans.worldPosition = mFerrisWheel.worldPosition + vector(-1152, -440, 1320)
  tColorCansPaint1 = mColorCans.shaderList[1].texture
  tColorCansPaint2 = mColorCans.shaderList[2].texture
  tColorSplash = w.model("TILE_FerrisWheelObjects_002").shaderList[13]
end

on doColorTank
  case colorTankState of
    0:
      tempDiff = mColorCans.worldPosition - gPlayerCar.worldPosition
      tempDiff.y = 0
      if abs(tempDiff.x) < 100 then
        if (tempDiff.z > 0) and (tempDiff.z < 300) then
          gPlayerCar.worldPosition.x = mColorCans.worldPosition.x
          mPlayerCar.transform.rotation = mPlayerCar.transform.rotation * 0.90000000000000002
          if (tempDiff.z < 160) and (g_DamagePercent < 1.0) then
            gPlayerCar.worldPosition.z = mColorCans.worldPosition.z - 160
            velVector.z = 0
            colorTankState = 1
            sprite(1).doPanel(7, rgb(255, 255, 255), 0)
          end if
        end if
      end if
    1:
      gPlayerCar.worldPosition.x = mColorCans.worldPosition.x
      mPlayerCar.transform.rotation = vector(0, 0, 0)
      if (mColorCans.worldPosition.z - gPlayerCar.worldPosition.z) > 160 then
        colorTanksState = 0
      else
        gPlayerCar.worldPosition.z = mColorCans.worldPosition.z - 160
      end if
      if keyPressed(125) then
        colorTankState = 0
        sprite(1).doPanel(0, rgb(255, 255, 255), 0)
        doSaveGameData()
      else
        if keyPressed(123) then
          sprite(1).doPanel(0, rgb(255, 255, 255), 0)
          colorSplashPercent = 0
          colorRot = -45
          colorTargetRot = -45
          colorPrevRot = -45
          tempPrevColorSplashID = colorSplashID
          colorSplashID = colorSplashID - 1
          colorCalcAngle = 0
          if colorSplashID < 1 then
            colorSplashID = 8
          end if
          mColorCans.shaderList[2].textureTransform.position = vector(0, 0, 0)
          mColorCans.shaderList[2].texture.member = member(170 + colorSplashID, "Textures")
          mColorCans.shaderList[1].texture.member = member(170 + tempPrevColorSplashID, "Textures")
          mColorCans.shaderList[1].textureTransform.position = vector(0, 0.5, 0)
          tColorSplash.texture.member = member(160 + colorSplashID, "Textures")
          colorTankState = 2
          tColorSplash.textureTransform.position = vector(0, 0, 0)
          mColorCans.transform.rotation = vector(0, colorRot, 0)
        else
          if keyPressed(124) then
            sprite(1).doPanel(0, rgb(255, 255, 255), 0)
            colorSplashPercent = 0
            colorRot = 0
            colorTargetRot = 45
            colorPrevRot = 0
            colorPrevRot = colorRot
            colorCalcAngle = 0
            tempPrevColorSplashID = colorSplashID
            colorSplashID = colorSplashID + 1
            if colorSplashID > 8 then
              colorSplashID = 1
            end if
            mColorCans.shaderList[1].textureTransform.position = vector(0, 0, 0)
            mColorCans.shaderList[1].texture.member = member(170 + colorSplashID, "Textures")
            mColorCans.shaderList[2].texture.member = member(170 + tempPrevColorSplashID, "Textures")
            mColorCans.shaderList[2].textureTransform.position = vector(0, 0.5, 0)
            tColorSplash.texture.member = member(160 + colorSplashID, "Textures")
            colorTankState = 2
            tColorSplash.textureTransform.position = vector(0, 0, 0)
            mColorCans.transform.rotation = vector(0, colorRot, 0)
          end if
        end if
      end if
    2:
      gPlayerCar.worldPosition.z = mColorCans.worldPosition.z - 160
      gPlayerCar.worldPosition.x = mColorCans.worldPosition.x
      mPlayerCar.transform.rotation = vector(0, 0, 0)
      velVector = vector(0, 0, 0)
      colorCalcAngle = colorCalcAngle + (5 * g_FPSFactor)
      if colorCalcAngle >= 180 then
        colorCalcAngle = 180
        colorTankState = 3
      end if
      colorRot = colorPrevRot + (colorTargetRot * ((cos(colorCalcAngle / radians) - 1) / 2.0))
      mColorCans.transform.rotation = vector(0, colorRot, 0)
    3:
      gPlayerCar.worldPosition.z = mColorCans.worldPosition.z - 160
      gPlayerCar.worldPosition.x = mColorCans.worldPosition.x
      mPlayerCar.transform.rotation = vector(0, 0, 0)
      velVector = vector(0, 0, 0)
      tempPrevColorSplashPercent = colorSplashPercent
      colorSplashPercent = colorSplashPercent + (0.01 * g_FPSFactor)
      if colorSplashPercent >= 1.0 then
        colorSplashPercent = 1.0
        colorTankState = 1
      end if
      if (tempPrevColorSplashPercent < 0.5) and (colorSplashPercent >= 0.5) then
        g_CarsColorIDList[playerCarID] = colorSplashID
        case playerCarID of
          1:
            mPlayerCar.shaderList[4].texture.member = member(150 + colorSplashID, "Textures")
          2:
            mPlayerCar.shaderList[10].texture.member = member(150 + colorSplashID, "Textures")
          3:
            mPlayerCar.shaderList[1].texture.member = member(150 + colorSplashID, "Textures")
          4:
            mPlayerCar.shaderList[10].texture.member = member(150 + colorSplashID, "Textures")
          5:
            mPlayerCar.shaderList[2].texture.member = member(150 + colorSplashID, "Textures")
          6:
            mPlayerCar.shaderList[5].texture.member = member(150 + colorSplashID, "Textures")
        end case
      end if
      mColorCans.shaderList[1].textureTransform.position = vector(0, colorSplashPercent * 0.5 * g_FPSFactor, 0)
      mColorCans.shaderList[2].textureTransform.position = vector(0, colorSplashPercent * 0.5 * g_FPSFactor, 0)
      tColorSplash.textureTransform.position = vector(0, colorSplashPercent, 0)
      gPlayerCar.worldPosition.z = mColorCans.worldPosition.z - 160
      mPlayerCar.transform.rotation = vector(0, 0, 0)
  end case
end

on doSetupRixRapidRepairs
  repairState = 0
  mRixGarageDoor = w.model("RixGarageDoor_000")
  mRixGarageDoor.worldPosition = mFerrisWheel.worldPosition + vector(1152, -378, 1050)
  mRixGarageDoor.transform.rotation = vector(0, 0, 0)
end

on doRixRapidRepairs
  tempDiff = mRixGarageDoor.worldPosition - gPlayerCar.worldPosition
  tempDiff.y = 0
  tempZ = tempDiff.z
  tempZ = tempZ - 100
  tempDiffZ = 65 - tempZ
  tempDoorOpenFactor = 1 - (abs(tempDiffZ) / 380.0)
  if tempDoorOpenFactor > 1 then
    tempDoorOpenFactor = 1
  end if
  if tempDoorOpenFactor < 0 then
    tempDoorOpenFactor = 0
  end if
  tempDiff = mRixGarageDoor.worldPosition - gPlayerCar.worldPosition
  tempDiff.y = 0
  mRixGarageDoor.transform.rotation = vector(90 * tempDoorOpenFactor, 0, 0)
  case repairState of
    0:
      if abs(tempDiff.x) < 200 then
        if (tempDiff.z > -420) and (tempDiff.z < 60) then
          sprite(1).doPanel(9, rgb(255, 255, 255), 0)
          repairState = 1
          gPlayerCar.worldPosition.x = mRixGarageDoor.worldPosition.x
          mPlayerCar.transform.rotation = mPlayerCar.transform.rotation * 0.94999999999999996
        end if
      end if
    1:
      if abs(tempDiff.x) < 200 then
        if (tempDiff.z > -420) and (tempDiff.z < 60) then
          gPlayerCar.worldPosition.x = mRixGarageDoor.worldPosition.x
          mPlayerCar.transform.rotation = mPlayerCar.transform.rotation * 0.94999999999999996
          if tempDiff.z < -210 then
            if g_Muted = 0 then
              if soundMemberList[1] <> member(24, "Sfx") then
                puppetSound(1, member(24, "Sfx"))
                soundMemberList[1] = member(24, "Sfx")
              end if
            end if
            gPlayerCar.worldPosition.z = mRixGarageDoor.worldPosition.z + 210
            velVector.z = 0
            g_DamagePercent = 0
            partRotList = [0, 0, 0, 0, 0, 0, 0]
            partRotVelList = [0, 0, 0, 0, 0, 0, 0]
          end if
        end if
      else
        repairState = 0
        sprite(1).doPanel(0, rgb(255, 255, 255), 0)
        doSaveGameData()
      end if
  end case
  prevDoorOpenFactor = tempDoorOpenFactor
end

on doResetFerrisWheel
  ferrisState = 0
  ferrisCalcAngle = 0
  ferrisCurrentCageID = 1
  ferrisCageCarIDList = [0, 0, 0, 0, 0, 0]
  ferrisRot = 180.0
  ferrisTargetRot = 180.0
  ferrisRotVel = 0.0
  prevferrisRotVel = 0.0
  ferrisSwing = 0.0
  ferrisSwingVel = 0.0
  ferrisRotAmount = 0.0
  prevFerrisRotAmount = 0.0
  nextFreeCageID = 2
  repeat with i = 1 to 6
    if g_CarsOwnedList[i] = 1 then
      ferrisCageCarIDList[nextFreeCageID] = i
      mFerrisCageList[nextFreeCageID].addChild(mShellList[i])
      nextFreeCageID = nextFreeCageID + 1
      mShellList[i].transform.position = vector(0, -135, 0)
      mShellList[i].transform.rotation = vector(0, 0, 0)
      next repeat
    end if
    if g_CarsOwnedList[i] = 0 then
      mShellList[i].removeFromWorld()
    end if
  end repeat
  mShellList[1].shaderList[4].texture.member = member(150 + g_CarsColorIDList[1], "Textures")
  mShellList[2].shaderList[10].texture.member = member(150 + g_CarsColorIDList[2], "Textures")
  mShellList[3].shaderList[1].texture.member = member(150 + g_CarsColorIDList[3], "Textures")
  mShellList[4].shaderList[10].texture.member = member(150 + g_CarsColorIDList[4], "Textures")
  mShellList[5].shaderList[2].texture.member = member(150 + g_CarsColorIDList[5], "Textures")
  mShellList[6].shaderList[5].texture.member = member(150 + g_CarsColorIDList[6], "Textures")
  doFerrisSwing()
  put "FerrisWheelSpawned!"
  mIgNitroList[1].visibility = #front
  mIgNitroList[1].addToWorld()
  mIgNitroList[1].worldPosition = vector(34017.54000000000087311, 50, 32709.34000000000014552)
end

on doSetupFerrisWheel
  ferrisState = 0
  enterFerrisCarID = 0
  ferrisCalcAngle = 0
  ferrisCurrentCageID = 1
  ferrisCageCarIDList = [0, 0, 0, 0, 0, 0]
  ferrisRot = 180.0
  ferrisTargetRot = 180.0
  ferrisRotVel = 0.0
  prevferrisRotVel = 0.0
  ferrisSwing = 0.0
  ferrisSwingVel = 0.0
  ferrisRotAmount = 0.0
  prevFerrisRotAmount = 0.0
  mFerrisWheel = w.model("FerrisWheel_WHEEL_001")
  mFerrisAxleList = [w.model("FerrisWheel_AXLE_000")]
  mFerrisCageList = [w.model("FerrisWheel_CAGE_001")]
  repeat with i = 2 to 6
    mFerrisAxleList.add(mFerrisAxleList[1].clone("FerrisAxleClone " & i))
    mFerrisCageList.add(mFerrisCageList[1].clone("FerrisCageClone" & i))
    mFerrisCageList[i].transform.rotation = vector(0, 180, 0)
  end repeat
  repeat with i = 1 to 6
    mFerrisAxleList[i].addChild(mFerrisCageList[i])
    mFerrisWheel.addChild(mFerrisAxleList[i])
    tempVector = vector(0, 0, 0)
    tempVector.x = 300 * cos((30 + (i * 60)) / radians)
    tempVector.y = 300 * sin((30 + (i * 60)) / radians)
    mFerrisAxleList[i].transform.position = tempVector
    mFerrisAxleList[i].transform.rotation = vector(0, 0, 180)
  end repeat
  mFerrisWheel.worldPosition = vector(32500, 440, 32500)
  nextFreeCageID = 2
  repeat with i = 1 to 6
    if g_CarsOwnedList[i] = 1 then
      ferrisCageCarIDList[nextFreeCageID] = i
      mFerrisCageList[nextFreeCageID].addChild(mShellList[i])
      nextFreeCageID = nextFreeCageID + 1
      mShellList[i].transform.position = vector(0, -135, 0)
      mShellList[i].transform.rotation = vector(0, 0, 0)
      next repeat
    end if
    if g_CarsOwnedList[i] = -1 then
      mShellList[i].worldPosition = mFerrisWheel.worldPosition + vector(1160, -440, 1100)
      mShellList[i].transform.rotation = vector(0, 180, 0)
      mShellList[i].child[1].transform.rotation = vector(-60, 0, 0)
      next repeat
    end if
    if g_CarsOwnedList[i] = 0 then
      mShellList[i].removeFromWorld()
    end if
  end repeat
  mFerrisWheel.transform.rotation = vector(0, 0, ferrisRot)
  tPointerArrows = w.model("TILE_FerrisWheelObjects_002").shaderList[14].texture
end

on doFerrisSwing
  repeat with i = g_FPSFactorDec down to 0
    if i = 0 then
      ferrisSwingVel = ferrisSwingVel - (ferrisSwingVel * 0.02 * g_FPSFactorFrac)
      ferrisRotVel = ferrisRotVel - (ferrisRotVel * 0.04 * g_FPSFactorFrac)
      next repeat
    end if
    ferrisSwingVel = ferrisSwingVel - (ferrisSwingVel * 0.02)
    ferrisRotVel = ferrisRotVel - (ferrisRotVel * 0.04)
  end repeat
  ferrisSwingVel = ferrisSwingVel - (ferrisSwing * 0.005 * g_FPSFactor)
  ferrisSwing = ferrisSwing + ferrisSwingVel
  mFerrisWheel.transform.rotation = vector(0, 0, ferrisRot)
  repeat with i = 1 to 6
    mFerrisAxleList[i].transform.rotation = vector(0, 0, -ferrisRot + ferrisSwing)
    if mFerrisAxleList[i].worldPosition.y < 324 then
      tempXFactor = mFerrisWheel.worldPosition.x + 165 - mFerrisAxleList[i].worldPosition.x
      if tempXFactor > 330 then
        tempXFactor = 330
      end if
      if tempXFactor < 0 then
        tempXFactor = 0
      end if
      tempXFactor = tempXFactor / 330.0
      tempYRot = (tempXFactor * 360) - 180
      mFerrisCageList[i].transform.rotation = vector(0, tempYRot, 0)
      next repeat
    end if
    mFerrisCageList[i].transform.rotation = vector(0, 180, 0)
  end repeat
end

on doFerrisWheel
  if (raceState = 0) and (stageState = 0) then
    w.model("TILE_FerrisWheelObjects_002").shaderList[14].textureTransform.translate(0, -0.01, 0)
  else
    w.model("TILE_FerrisWheelObjects_002").shaderList[14].textureTransform.position = vector(0, 0, 0)
  end if
  case ferrisState of
    0:
      tempDiff = mFerrisWheel.worldPosition - gPlayerCar.worldPosition
      tempDiff.y = 0
      if (tempDiff.z < 200) and (tempDiff.z > -360) and (abs(tempDiff.x) < 100) and (g_DamagePercent < 1.0) then
        ferrisState = 1
      end if
      if tempDiff.magnitude < 4500 then
        mFerrisWheel.addToWorld()
        if raceState = 2 then
          sprite(1).doPanel(33, rgb(255, 255, 255), 200)
          currentRace = 0
          opponentState = 0
          raceState = 0
          racePodPointer = 0
          opponentDotID = 0
          opponentVelVector = vector(0, 0, 0)
          opponentRotVector = vector(0, 0, 0)
          mOpponentCar.removeFromWorld()
          gOpponentCar.transform.position = vector(-1000, -1000, -1000)
          mRacePod.removeFromWorld()
          mNextRacePod.removeFromWorld()
          repeat with i = 1 to mIgNitroList.count
            mIgNitroList[i].visibility = #none
          end repeat
          sprite(1).timeList = [0, 0, 0, 0, 0]
          g_TimerState = 0
          sprite(1).updateTimer()
          raceLightPhase = 0
          g_CurrentScore = 0
          raceFlagStartTicks = the ticks
          mRacePod.removeFromWorld()
          mNextRacePod.removeFromWorld()
          opponentSpawned = 0
        end if
        if ferrisWheelSpawned = 0 then
          doResetFerrisWheel()
          ferrisWheelSpawned = 1
        end if
      else
        ferrisWheelSpawned = 0
        mFerrisWheel.removeFromWorld()
      end if
    1:
      tempDiff = mFerrisWheel.worldPosition - gPlayerCar.worldPosition
      tempDiff.y = 0
      if (tempDiff.z < 200) and (tempDiff.z > -360) and (abs(tempDiff.x) < 100) then
        gPlayerCar.worldPosition.x = mFerrisWheel.worldPosition.x
        velVector.x = 0
        rotVector = vector(0, 0, 0)
        mPlayerCar.transform.rotation = mPlayerCar.transform.rotation * 0.94999999999999996
        if gPlayerCar.worldPosition.z > mFerrisWheel.worldPosition.z then
          carDamageList[playerCarID] = g_DamagePercent
          carBoostList[playerCarID] = g_BoostPercent
          gPlayerCar.worldPosition.z = mFerrisWheel.worldPosition.z
          sprite(1).doPanel(8, rgb(255, 255, 255), 0)
          doSaveGameData()
          ferrisState = 2
          if g_Muted = 0 then
            if soundBusy(1) = 0 then
              puppetSound(1, member(21, "Sfx"))
              soundMemberList[1] = member(21, "Sfx")
            end if
          end if
          enterFerrisCarID = playerCarID
          ferrisCageCarIDList[ferrisCurrentCageID] = playerCarID
          mFerrisAxleList[ferrisCurrentCageID].child[1].addChild(mPlayerCar)
          mPlayerCar.transform.rotation = vector(0, 0, 0)
          mPlayerCar.transform.position = vector(0, -135, 0)
          mPlayerCar = gCarPlaceHolder
          gPlayerCar.addChild(mPlayerCar)
          mPlayerCar.transform.position = vector(0, 0, 0)
          mPlayerCar.transform.rotation = vector(0, 0, 0)
          gPlayerCar.addChild(gPlayerCage)
          gPlayerCage.transform.position = vector(0, 0, 0)
          mTireFL.transform.position.y = 16
          mTireBL.transform.position.y = 16
          mTireFR.transform.position.y = 16
          mTireBR.transform.position.y = 16
          mBonnet = gCarPlaceHolder
          mBoot = gCarPlaceHolder
          mDoorL = gCarPlaceHolder
          mTireFL = gTirePlaceHolder
          mTireBL = gTirePlaceHolder
          mDoorR = gCarPlaceHolder
          mTireFR = gTirePlaceHolder
          mTireBR = gTirePlaceHolder
          partRotVelList = [0, 0, 0, 0, 0, 0, 0, 0]
          partRotList = [0, 0, 0, 0, 0, 0, 0, 0]
        end if
      end if
      doFerrisSwing()
    2:
      velVector = vector(0, 0, 0)
      rotVector = vector(0, 0, 0)
      gPlayerCar.worldPosition.x = mFerrisWheel.worldPosition.x
      gPlayerCar.worldPosition.z = mFerrisWheel.worldPosition.z
      if keyPressed(125) and not keyPressed(126) then
        sprite(1).doPanel(0, rgb(200, 200, 255), 0)
        ferrisState = 0
        g_CarsOwnedList[enterFerrisCarID] = 1
        playerCarID = ferrisCageCarIDList[ferrisCurrentCageID]
        g_CarsOwnedList[playerCarID] = 2
        doSaveGameData()
        tempRot = mPlayerCar.transform.rotation.duplicate()
        mPlayerCar = mShellList[playerCarID]
        gPlayerCar.addChild(mPlayerCar)
        mPlayerCar.transform.position = vector(0, 0, 0)
        mPlayerCar.transform.rotation = tempRot
        mPlayerCar.addChild(gPlayerCage)
        gPlayerCage.transform.position = vector(0, 0, 0)
        gPlayerCage.transform.rotation = vector(0, 0, 0)
        mPlayerCar.addChild(mIgNitroBlastL)
        mPlayerCar.addChild(mIgNitroBlastR)
        mPlayerCar.addChild(gIgNitro)
        gIgNitro.addChild(mIgNitro)
        mIgNitroBlastL.transform.rotation = vector(10, 0, 0)
        mIgNitroBlastR.transform.rotation = vector(10, 0, 0)
        case playerCarID of
          1:
            mIgNitroBlastL.transform.position = vector(30, 20, -95)
            mIgNitroBlastL.transform.position = vector(30, 20, -95)
            gIgNitro.transform.position = vector(0, 85, -80)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
          2:
            mIgNitroBlastL.transform.position = vector(-30, 25, -125)
            mIgNitroBlastR.transform.position = vector(30, 25, -125)
            gIgNitro.transform.position = vector(0, 96, -110)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
          3:
            mIgNitroBlastL.transform.position = vector(-38, 25, -125)
            mIgNitroBlastR.transform.position = vector(38, 25, -125)
            gIgNitro.transform.position = vector(0, 92, -110)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
          4:
            mIgNitroBlastL.transform.position = vector(-30, 22, -120)
            mIgNitroBlastR.transform.position = vector(30, 22, -120)
            gIgNitro.transform.position = vector(0, 94, -95)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
          5:
            mIgNitroBlastL.transform.position = vector(-36, 20, -140)
            mIgNitroBlastR.transform.position = vector(36, 20, -140)
            gIgNitro.transform.position = vector(0, 92, -120)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
          6:
            mIgNitroBlastL.transform.position = vector(-45, 25, -120)
            mIgNitroBlastR.transform.position = vector(45, 25, -120)
            gIgNitro.transform.position = vector(0, 105, -70)
            gIgNitro.transform.rotation = vector(0, 0, 0)
            mIgNitro.transform.position = vector(0, -50, 0)
            mIgNitro.transform.rotation = vector(0, 0, 0)
        end case
        mBonnet = mPlayerCar.child[1]
        mBoot = mPlayerCar.child[2]
        mDoorL = mPlayerCar.child[3]
        mTireFL = mPlayerCar.child[4]
        mTireBL = mPlayerCar.child[5]
        mDoorR = mPlayerCar.child[6].child[1]
        mTireFR = mPlayerCar.child[6].child[2]
        mTireBR = mPlayerCar.child[6].child[3]
      else
        if keyPressed(124) then
          tempCarID = ferrisCurrentCageID + 1
          if tempCarID > 6 then
            tempCarID = 1
          end if
          if ferrisCageCarIDList[tempCarID] > 0 then
            ferrisCurrentCageID = ferrisCurrentCageID + 1
            if ferrisCurrentCageID > 6 then
              ferrisCurrentCageID = 1
            end if
            playerCarID = ferrisCageCarIDList[ferrisCurrentCageID]
            g_DamagePercent = carDamageList[playerCarID]
            g_BoostPercent = carBoostList[playerCarID]
            ferrisState = 3
            ferrisPrevRot = ferrisRot
            ferrisTargetRot = 60
            ferrisCalcAngle = 0
            if g_Muted = 0 then
              puppetSound(2, member(22, "Sfx"))
              soundMemberList[2] = member(22, "Sfx")
            end if
          end if
          sprite(1).doPanel(0, rgb(255, 200, 0), 0)
        end if
        if keyPressed(123) then
          tempCarID = ferrisCurrentCageID - 1
          if tempCarID < 1 then
            tempCarID = 6
          end if
          if ferrisCageCarIDList[tempCarID] > 0 then
            ferrisCurrentCageID = ferrisCurrentCageID - 1
            if ferrisCurrentCageID < 1 then
              ferrisCurrentCageID = 6
            end if
            playerCarID = ferrisCageCarIDList[ferrisCurrentCageID]
            g_DamagePercent = carDamageList[playerCarID]
            g_BoostPercent = carBoostList[playerCarID]
            ferrisState = 3
            ferrisPrevRot = ferrisRot
            ferrisTargetRot = -60
            ferrisCalcAngle = 0
            if g_Muted = 0 then
              puppetSound(2, member(22, "Sfx"))
              soundMemberList[2] = member(22, "Sfx")
            end if
          end if
          sprite(1).doPanel(0, rgb(255, 255, 255), 0)
        end if
      end if
      doFerrisSwing()
    3:
      velVector = vector(0, 0, 0)
      rotVector = vector(0, 0, 0)
      gPlayerCar.worldPosition.x = mFerrisWheel.worldPosition.x
      gPlayerCar.worldPosition.z = mFerrisWheel.worldPosition.z
      ferrisCalcAngle = ferrisCalcAngle + (1.39999999999999991 * g_FPSFactor)
      if ferrisCalcAngle >= 180 then
        ferrisCalcAngle = 180
        ferrisState = 2
        sprite(1).doPanel(playerCarID, rgb(255, 255, 255), 0)
      end if
      tempPrevFerrisRot = ferrisRot
      ferrisRot = ferrisPrevRot + (ferrisTargetRot * ((cos(ferrisCalcAngle / radians) - 1) / 2.0))
      ferrisSwingVel = ferrisSwingVel + ((tempPrevFerrisRot - ferrisRot) * 0.20000000000000001)
      doFerrisSwing()
      prevferrisRotVel = ferrisRotVel
  end case
end

on doSetupShells
  repeat with i = 1 to 6
    if g_CarsOwnedList[i] = 2 then
      playerCarID = i
    end if
  end repeat
  mShellList = []
  mShellList = [w.model("MeanieCropper_BODY_000"), w.model("Bitsasushi_BODY_000"), w.model("Amazed_BODY_012"), w.model("HighWire_BODY_000"), w.model("Posher_BODY_007"), w.model("Furore_BODY_000")]
  tempiCount = mShellList.count
  repeat with i = 1 to tempiCount
    tempCar = mShellList[i]
    tempBonnet = mShellList[i].child[1]
    tempBoot = mShellList[i].child[2]
    tempDoorL = mShellList[i].child[3]
    tempTireFL = mShellList[i].child[4]
    tempTireBL = tempTireFL.clone("TireBLClone_" & i)
    case i of
      1:
        tempTireBL.translate(0, 0, -130, #self)
      2:
        tempTireBL.translate(2, 0, -150, #self)
      3:
        tempTireBL.translate(2, 0, -160, #self)
      4:
        tempTireBL.translate(2, 0, -134, #self)
      5:
        tempTireBL.translate(2, 0, -150, #self)
      6:
        tempTireBL.translate(2, 0, -150, #self)
    end case
    tempCarMirror = tempCar.clone("CarMirrorClone_" & i)
    tempCar.addChild(tempCarMirror)
    tempCarMirror.transform.scale = vector(-1, 1, 1)
    tempBoot.addChild(tempCarMirror.child[2], #preserveWorld)
    tempBonnet.addChild(tempCarMirror.child[1], #preserveWorld)
    tempDoorR = tempCarMirror.child[1]
    tempTireFR = tempCarMirror.child[2]
    tempTireBR = tempCarMirror.child[3]
  end repeat
  mPlayerCar = mShellList[playerCarID]
  mBonnet = mPlayerCar.child[1]
  mBoot = mPlayerCar.child[2]
  mDoorL = mPlayerCar.child[3]
  mTireFL = mPlayerCar.child[4]
  mTireBL = mPlayerCar.child[5]
  mDoorR = mPlayerCar.child[6].child[1]
  mTireFR = mPlayerCar.child[6].child[2]
  mTireBR = mPlayerCar.child[6].child[3]
  gPlayerCar = w.newGroup("PlayerCarGroup")
  gPlayerCar.addChild(mPlayerCar)
  mPlayerCar.transform.position = vector(0, 0, 0)
  mShellList[1].shaderList[4].texture.member = member(150 + g_CarsColorIDList[1], "Textures")
  mShellList[2].shaderList[10].texture.member = member(150 + g_CarsColorIDList[2], "Textures")
  mShellList[3].shaderList[1].texture.member = member(150 + g_CarsColorIDList[3], "Textures")
  mShellList[4].shaderList[10].texture.member = member(150 + g_CarsColorIDList[4], "Textures")
  mShellList[5].shaderList[2].texture.member = member(150 + g_CarsColorIDList[5], "Textures")
  mShellList[6].shaderList[5].texture.member = member(150 + g_CarsColorIDList[6], "Textures")
  mOpponentShellList = []
  repeat with i = 2 to 6
    mOpponentShellList.add(mShellList[i].cloneDeep("OpponentShellClone" && i))
    mOpponentShellList[i - 1].removeFromWorld()
  end repeat
end

on doSetupPlane
  mPlane = w.model("TrafficBoeing747_001")
  gPlane = w.newGroup("planeGroup")
  gPlane.addChild(mPlane)
  gPlaneCollider = w.newGroup("planeColliderGroup")
  mPlane.addChild(gPlaneCollider)
  planeSpeed = 0
  planeState = 7
  planeYRot = 0
  planeTilt = 0
  planeBank = 0
  airPortOrigin = vector(7500, 0, 7500)
  gPlane.worldPosition = airPortOrigin.duplicate()
  planeStartTicks = the ticks
end

on doPlane
  case planeState of
    1:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if tempDiff.z > 20000 then
        planeState = planeState + 1
      end if
      planeSpeed = planeSpeed + (0.05 * g_FPSFactor)
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, 0)
    2:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if tempDiff.z > 30000 then
        planeState = planeState + 1
      end if
      if planeTilt < 8 then
        planeTilt = planeTilt + (0.05 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed + (0.05 * g_FPSFactor)
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, 0)
    3:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if tempDiff.z > 40000 then
        planeState = planeState + 1
      end if
      if planeTilt < 20 then
        planeTilt = planeTilt + (0.05 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed + (0.05 * g_FPSFactor)
      gPlane.translate(0, 2 * g_FPSFactor, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, 0)
    4:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if tempDiff.z > 50000 then
        planeState = planeState + 1
      end if
      if planeTilt < 20 then
        planeTilt = planeTilt + (0.05 * g_FPSFactor)
      end if
      if planeBank < 30 then
        planeBank = planeBank + (0.05 * g_FPSFactor)
      end if
      gPlane.translate(0, 10 * g_FPSFactor, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
    5:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed + (0.05 * g_FPSFactor)
      planeYRot = planeYRot + (0.25 * g_FPSFactor)
      gPlane.translate(0, 10, planeSpeed * g_FPSFactor, #self)
      if planeBank < 30 then
        planeBank = planeBank + (0.10000000000000001 * g_FPSFactor)
      end if
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if planeYRot >= 90 then
        planeState = planeState + 1
        planeYRot = 90
      end if
    6:
      tempDiff = gPlane.worldPosition - airPortOrigin
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      else
        if planeTilt < 0 then
          planeTilt = 0
        end if
      end if
      if planeBank > 0 then
        planeBank = planeBank - (0.5 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed + (0.05 * g_FPSFactor)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      if tempDiff.x > 80000 then
        planeState = planeState + 1
      end if
    7:
      gPlane.worldPosition = airPortOrigin + vector(0, 1000, 70000)
      planeTilt = 0
      planeBank = 0
      planeSpeed = 60
      planeYRot = 180
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      planeState = planeState + 1
    8:
      if planeTilt < 10 then
        planeTilt = planeTilt + (0.05 * g_FPSFactor)
      end if
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if gPlane.worldPosition.z < 50000 then
        planeState = planeState + 1
      end if
    9:
      if planeTilt < 10 then
        planeTilt = planeTilt + (0.05 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed - (0.05 * g_FPSFactor)
      gPlane.translate(0, -5 * g_FPSFactor, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if gPlane.worldPosition.y <= 0 then
        gPlane.worldPosition.y = 0
        planeState = planeState + 1
      end if
    10:
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      end if
      planeSpeed = planeSpeed - (0.05 * g_FPSFactor)
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if planeTilt <= 0 then
        planeTilt = 0
        planeState = planeState + 1
      end if
    11:
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      end if
      if planeSpeed > 2 then
        planeSpeed = planeSpeed - (0.028 * g_FPSFactor)
      else
        planeSpeed = 2
      end if
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if gPlane.worldPosition.z < 6600 then
        planeState = planeState + 1
        planeSpeed = 5
      end if
    12:
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      end if
      if planeSpeed > 4 then
        planeSpeed = planeSpeed - (0.025 * g_FPSFactor)
      else
        planeSpeed = 4
      end if
      planeYRot = planeYRot - (0.25 * g_FPSFactor)
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if planeYRot <= 90 then
        planeYRot = 90
        planeSpeed = 4
        planeState = planeState + 1
        planeStartTicks = the ticks
      end if
    13:
      if planeTilt > 0 then
        planeTilt = planeTilt - (0.05 * g_FPSFactor)
      end if
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if gPlane.worldPosition.x > 9850 then
        planeSpeed = 0
        planeState = planeState + 1
        planeStartTicks = the ticks
      end if
    14:
      if (the ticks - planeStartTicks) > 200 then
        planeSpeed = -4
        planeState = planeState + 1
      end if
    15:
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
      if gPlane.worldPosition.x <= 8425.0 then
        planeSpeed = -4
        planeState = planeState + 1
        planeStartTicks = the ticks
      end if
    16:
      planeYRot = planeYRot - (0.25 * g_FPSFactor)
      if planeYRot <= 0 then
        planeYRot = 0
        planeSpeed = 0
        planeState = 1
        planeStartTicks = the ticks
        put "FYI-----Plane X = " && gPlane.worldPosition.x
      end if
      gPlane.translate(0, 0, planeSpeed * g_FPSFactor, #self)
      mPlane.transform.rotation = vector(-planeTilt, 0, -planeBank)
      gPlane.transform.rotation = vector(0, planeYRot, 0)
  end case
end

on doEngineSounds
  repeat with i = 1 to 6
    if soundBusy(i) = 0 then
      soundMemberList[i] = VOID
    end if
  end repeat
  upPressed = keyPressed(126)
  case engineState of
    0:
      if upPressed = 1 then
        engineState = 1
        tempStartTime = integer(member((playerCarID * 2) - 1, "Sfx").duration * (1 - soundCompletePercent))
        if tempStartTime < member((playerCarID * 2) - 1, "Sfx").duration then
          tempList = [#member: member((playerCarID * 2) - 1, "Sfx"), #startTime: tempStartTime]
        else
          tempList = [#member: member((playerCarID * 2) - 1, "Sfx")]
        end if
        if g_Muted = 0 then
          sound(2).play(tempList)
          soundMemberList[2] = tempList.member
        end if
      end if
    1:
      soundCompletePercent = sound(2).currentTime / member((playerCarID * 2) - 1, "Sfx").duration
      if (soundBusy(2) = 0) and (g_Muted = 0) then
        sound(2).play(member((playerCarID * 2) - 1, "Sfx"))
        soundMemberList[2] = member((playerCarID * 2) - 1, "Sfx")
      end if
      if upPressed = 0 then
        engineState = 2
        tempStartTime = integer(member(playerCarID * 2, "Sfx").duration * (1 - soundCompletePercent))
        if tempStartTime < member(playerCarID * 2, "Sfx").duration then
          tempList = [#member: member(playerCarID * 2, "Sfx"), #startTime: tempStartTime]
        else
          tempList = [#member: member(playerCarID * 2, "Sfx")]
        end if
        if g_Muted = 0 then
          sound(2).play(tempList)
          soundMemberList[2] = tempList.member
        end if
      end if
    2:
      soundCompletePercent = sound(2).currentTime / member(playerCarID * 2, "Sfx").duration
      if (soundCompletePercent >= 1.0) or (soundBusy(2) = 0) then
        soundCompletePercent = 1.0
        engineState = 0
      end if
      if upPressed = 1 then
        engineState = 1
        tempStartTime = integer(member((playerCarID * 2) - 1, "Sfx").duration * (1 - soundCompletePercent))
        if tempStartTime < member((playerCarID * 2) - 1, "Sfx").duration then
          tempList = [#member: member((playerCarID * 2) - 1, "Sfx"), #startTime: tempStartTime]
        else
          tempList = [#member: member((playerCarID * 2) - 1, "Sfx")]
        end if
        if g_Muted = 0 then
          sound(2).play(tempList)
          soundMemberList[2] = tempList.member
        end if
      end if
  end case
  prevUpPressed = upPressed
end

on doSetupTallObjects
  mTemp = w.model("TALLOBJECT_SunnyPierceToole_000")
  mTemp.worldPosition = vector(17500, 115, 1000)
  mTemp.rotate(0, 0, 0, #self)
  mTemp = w.model("TALLOBJECT_GoldenGateBridge_001")
  mTemp.transform.position = vector(57500, 0, 27500)
  mTemp.addChild(mTemp.clone("GoldenGateBridgeClone"))
  mTemp.child[1].rotate(0, 180, 0)
  mTemp.child[1].translate(0, 0, 10000, #self)
  mTemp = w.model("TALLOBJECT_WellardScrewDriver_000")
  mTemp.worldPosition = vector(27500, 115, 800)
  mTemp.rotate(0, 0, 0, #self)
  mTemp = w.model("TALLOBJECT_TahTeeCosmetics_000")
  mTemp.worldPosition = vector(22500, 0, 12500)
  mTemp.rotate(0, 90, 0, #self)
  mTemp = w.model("TALLOBJECT_Lunkopf&StubbsCigarette_000")
  mTemp.worldPosition = vector(22500, 115, 800)
  mTemp.rotate(0, 0, 0, #self)
  mRadar = w.model("AirPortRadar_000")
  mRadar.worldPosition = vector(3500, 0, 13500)
end

on doSetupTrafficLights
  trafficLightPhase = 1
  tempiCount = redTrafficLightIDList.count
  repeat with i = 1 to tempiCount
    if dotOccupiedList[redTrafficLightIDList[i]] > 0 then
      dotOccupiedList[redTrafficLightIDList[i]] = -dotOccupiedList[redTrafficLightIDList[i]]
      next repeat
    end if
    dotOccupiedList[redTrafficLightIDList[i]] = -1111
  end repeat
end

on doTrafficLights
  case trafficLightPhase of
    1:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 255, 0)
      if (the ticks - trafficLightStartTicks) > 400 then
        trafficLightPhase = 2
        trafficLightStartTicks = the ticks
        tempiCount = greenTrafficLightIDList.count
        repeat with i = 1 to tempiCount
          if dotOccupiedList[greenTrafficLightIDList[i]] > 0 then
            dotOccupiedList[greenTrafficLightIDList[i]] = -dotOccupiedList[greenTrafficLightIDList[i]]
            next repeat
          end if
          dotOccupiedList[greenTrafficLightIDList[i]] = -1111
        end repeat
      end if
    2:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(255, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(255, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 55, 0)
      if (the ticks - trafficLightStartTicks) > 100 then
        trafficLightPhase = 3
        trafficLightStartTicks = the ticks
      end if
    3:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 55, 0)
      if (the ticks - trafficLightStartTicks) > 100 then
        trafficLightPhase = 4
        trafficLightStartTicks = the ticks
        tempiCount = redTrafficLightIDList.count
        repeat with i = 1 to tempiCount
          if dotOccupiedList[redTrafficLightIDList[i]] < 0 then
            if dotOccupiedList[redTrafficLightIDList[i]] = -1111 then
              dotOccupiedList[redTrafficLightIDList[i]] = 0
              next repeat
            end if
            dotOccupiedList[redTrafficLightIDList[i]] = -dotOccupiedList[redTrafficLightIDList[i]]
          end if
        end repeat
      end if
    4:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 55, 0)
      if (the ticks - trafficLightStartTicks) > 400 then
        trafficLightPhase = 5
        trafficLightStartTicks = the ticks
        tempiCount = redTrafficLightIDList.count
        repeat with i = 1 to tempiCount
          if dotOccupiedList[redTrafficLightIDList[i]] > 0 then
            dotOccupiedList[redTrafficLightIDList[i]] = -dotOccupiedList[redTrafficLightIDList[i]]
            next repeat
          end if
          dotOccupiedList[redTrafficLightIDList[i]] = -1111
        end repeat
      end if
    5:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(55, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(255, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(255, 255, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 55, 0)
      if (the ticks - trafficLightStartTicks) > 100 then
        trafficLightPhase = 6
        trafficLightStartTicks = the ticks
      end if
    6:
      w.shader("Texture_Tile_TrafficLightPhase1Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase1Green").diffuse = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").emissive = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Red").diffuse = rgb(255, 0, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").emissive = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Amber").diffuse = rgb(55, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").emissive = rgb(0, 55, 0)
      w.shader("Texture_Tile_TrafficLightPhase2Green").diffuse = rgb(0, 55, 0)
      if (the ticks - trafficLightStartTicks) > 100 then
        trafficLightPhase = 1
        trafficLightStartTicks = the ticks
        tempiCount = greenTrafficLightIDList.count
        repeat with i = 1 to tempiCount
          if dotOccupiedList[greenTrafficLightIDList[i]] < 0 then
            if dotOccupiedList[greenTrafficLightIDList[i]] = -1111 then
              dotOccupiedList[greenTrafficLightIDList[i]] = 0
              next repeat
            end if
            dotOccupiedList[greenTrafficLightIDList[i]] = -dotOccupiedList[greenTrafficLightIDList[i]]
          end if
        end repeat
      end if
  end case
end

on doSetupOpponent
  gOpponentCar = w.newGroup("OpponentGroup")
  mOpponentCar = mOpponentShellList[1]
  gOpponentCar.addChild(mOpponentCar)
  gOpponentCar.worldPosition = vector(0, -10000000, 0)
  opponentState = 0
  gOpponentCageList = []
  opponentIsectPosList = []
  opponentIsectPosList[4] = vector(0, 0, 0)
  opponentisectNormList = []
  opponentisectNormList[4] = vector(0, 0, 0)
  opponentIList = []
  opponentIList[4] = 0
  opponentIIDList = []
  opponentIIDList[4] = 1
  opponentICapList = []
  opponentICapList[4] = 0
  opponentImbedList = []
  opponentImbedList[4] = 0
  opponentImbedRotList = []
  opponentImbedRotList[4] = 0
  opponentImbedIDList = []
  opponentImbedIDList[4] = 0
  opponentImbedCapList = []
  opponentImbedCapList[4] = 0
  opponentToCenterList = []
  opponentToCenterList[4] = vector(0, 0, 0)
  opponentVelVector = vector(0, 0, 0)
  prevOpponentVelVector = vector(0, 0, 0)
  opponentRotVector = vector(0, 0, 0)
  opponentGripVector = vector(0, 0, 0)
  opponentThrottle = 0
  opponentSteer = 0
  opponentMidAir = 0
  opponentHoist = 0
  opponentHoistCap = 0
  opponentTargetVel = 100
  opponentTileIDList = []
  opponentTileRotList = []
  opponentTileRowList = []
  opponentTileColList = []
  opponentOnTileX = 0
  opponentOnTileZ = 0
  opponentOnTile = 0
  opponentOnTileID = 0
  opponentOnTileCol = 1
  opponentOnTileRow = 1
  opponentDotID = 1
  opponentTargetPos = vector(0, 0, 0)
  gOpponentCage = w.newGroup("OpponentCageGroup")
  repeat with i = 1 to 4
    gOpponentCageList.add(w.newGroup("opponentCagePointGroup" & i))
    gOpponentCage.addChild(gOpponentCageList[i])
  end repeat
  gOpponentCageList[1].transform.position = vector(-50, 30, -60)
  gOpponentCageList[2].transform.position = vector(50, 30, -60)
  gOpponentCageList[3].transform.position = vector(-50, 30, 60)
  gOpponentCageList[4].transform.position = vector(50, 30, 60)
  gOpponentCar.worldPosition = vector(0, 0, -1000)
end

on doOpponent
  case opponentState of
    0:
    1:
      repeat with i = 1 to 4
        opponentTileColList[i] = integer((gOpponentCageList[i].worldPosition.x / 5000.0) + 0.5)
        opponentTileRowList[i] = integer((gOpponentCageList[i].worldPosition.z / 5000.0) + 0.5)
        opponentImbedIDList[i] = tileIDList[opponentTileColList[i] + ((opponentTileRowList[i] - 1) * tileCols)]
        opponentImbedRotList[i] = tileRotList[opponentTileColList[i] + ((opponentTileRowList[i] - 1) * tileCols)]
      end repeat
      opponentOnTileRow = integer((gPlayerCar.worldPosition.z / 5000.0) + 0.5)
      opponentOnTileCol = integer((gPlayerCar.worldPosition.x / 5000.0) + 0.5)
      opponentOnTile = opponentOnTileCol + ((opponentOnTileRow - 1) * tileCols)
      opponentOnTileID = tileIDList[opponentOnTile]
      opponentOnTileX = gOpponentCar.worldPosition.x mod 5000
      opponentOnTileZ = gOpponentCar.worldPosition.z mod 5000
      opponentHoist = 0
      opponentMidAir = 1
      repeat with i = 1 to 4
        tempVector = vector(0, 0, 0)
        tempVector.x = gOpponentCageList[i].worldPosition.x mod 5000
        tempVector.y = gOpponentCageList[i].worldPosition.y + 60
        tempVector.z = gOpponentCageList[i].worldPosition.z mod 5000
        if tempVector.x < 0.01 then
          tempVector.x = 0.01
        end if
        if tempVector.z < 0.01 then
          tempVector.z = 0.01
        end if
        if tempVector.x > 4999.98999999999978172 then
          tempVector.x = 4999.98999999999978172
        end if
        if tempVector.z > 4999.98999999999978172 then
          tempVector.z = 4999.98999999999978172
        end if
        tempModelList = [mTileResourceList[opponentImbedIDList[i]]]
        tempOptionsList = [#modelList: tempModelList, #levelOfDetail: #detailed]
        mTileResourceList[opponentImbedIDList[i]].addToWorld()
        mTileResourceList[opponentImbedIDList[i]].transform.rotation = vector(0, -90 * opponentImbedRotList[i], 0)
        tempResult = w.modelsUnderRay(tempVector, vector(0, -1, 0), tempOptionsList)
        if tempResult.count > 0 then
          tempResult = tempResult[1]
          opponentisectNormList[i] = tempResult.isectNormal
          opponentIsectPosList[i] = tempResult.isectPosition
          opponentImbedList[i] = opponentIsectPosList[i].y - gOpponentCageList[i].worldPosition.y + 30
          if opponentImbedList[i] < 0 then
            opponentImbedList[i] = 0
          else
            opponentMidAir = 0
          end if
          opponentImbedCapList[i] = opponentImbedList[i]
          opponentToCenterList[i] = gOpponentCageList[i].worldPosition - gOpponentCar.worldPosition
          if opponentImbedCapList[i] > 20 then
            if opponentHoist < (opponentImbedList[i] - 20) then
              opponentHoist = opponentImbedList[i] - 20
            end if
            opponentImbedCapList[i] = 20
          end if
        else
        end if
        mTileResourceList[opponentImbedIDList[i]].removeFromWorld()
      end repeat
      repeat with i = 1 to 4
        opponentVelVector.y = opponentVelVector.y + (opponentImbedCapList[i] * 0.045 * g_FPSFactor)
        opponentRotVector.x = opponentRotVector.x - (opponentToCenterList[i].z * opponentImbedCapList[i] * 0.0008 * g_FPSFactor)
        opponentRotVector.z = opponentRotVector.z + (opponentToCenterList[i].x * opponentImbedCapList[i] * 0.0008 * g_FPSFactor)
        if opponentImbedList[i] > 0 then
          opponentSlideVector = opponentisectNormList[i].duplicate()
          opponentVelVector.z = opponentVelVector.z + (opponentisectNormList[i].z * 0.10000000000000001 * g_FPSFactor)
          opponentVelVector.x = opponentVelVector.x + (opponentisectNormList[i].x * 0.10000000000000001 * g_FPSFactor)
        end if
      end repeat
      opponentHoistCap = opponentHoist
      tempDiffVector = gOpponentCar.worldPosition - opponentTargetPos
      tempDiffVector.y = 0
      if tempDiffVector.magnitude < 800 then
        tempPos = opponentTargetPos
        opponentDotID = opponentDotID + 1
        if redDotPosList[currentRace].count >= opponentDotID then
          opponentTargetPos = redDotPosList[currentRace][opponentDotID]
          tempDist = (tempPos - opponentTargetPos).magnitude
          opponentTargetVel = tempDist * 0.04
        end if
      end if
      tempSteer = mOpponentCar.transform.xAxis.dot(-tempDiffVector)
      tempDotBehindOpponent = mOpponentCar.transform.zAxis.dot(-tempDiffVector)
      if tempDotBehindOpponent < 0 then
        if tempSteer > 0 then
          tempSteer = 1
        else
          tempSteer = -1
        end if
      end if
      if tempSteer > 0.20000000000000001 then
        if opponentSteer < 0 then
          opponentSteer = 0
        end if
        if opponentSteer < 40 then
          opponentSteer = opponentSteer + (2 * g_FPSFactor)
        end if
      else
        if tempSteer < -0.20000000000000001 then
          if opponentSteer > 0 then
            opponentSteer = 0
          end if
          if opponentSteer > -40 then
            opponentSteer = opponentSteer - (2 * g_FPSFactor)
          end if
        else
          opponentSteer = opponentSteer * 0.90000000000000002
        end if
      end if
      if opponentVelVector.magnitude < opponentTargetVel then
        opponentThrottle = opponentThrottle + (0.20000000000000001 * g_FPSFactor)
      else
        opponentThrottle = 0
      end if
      if opponentThrottle < -8 then
        opponentThrottle = -8
      end if
      if opponentThrottle > 10 then
        opponentThrottle = 10
      end if
      if tempDotBehindOpponent < 0 then
        opponentThrottle = 2
      end if
      if raceState > 2 then
        opponentThrottle = 0
        opponentSteer = 0
        repeat with i = g_FPSFactorDec down to 0
          if i = 0 then
            opponentVelVector = opponentVelVector - (opponentVelVector * 0.05 * g_FPSFactorFrac)
            next repeat
          end if
          opponentVelVector = opponentVelVector - (opponentVelVector * 0.05)
        end repeat
      end if
      opponentThrottleVector = -mOpponentCar.transform.zAxis
      if opponentMidAir = 0 then
        case currentRace + 1 of
          2:
            opponentVelVector = opponentVelVector - (opponentThrottleVector * opponentThrottle * 0.03 * g_FPSFactor)
          3:
            opponentVelVector = opponentVelVector - (opponentThrottleVector * opponentThrottle * 0.032 * g_FPSFactor)
          4:
            opponentVelVector = opponentVelVector - (opponentThrottleVector * opponentThrottle * 0.036 * g_FPSFactor)
          5:
            opponentVelVector = opponentVelVector - (opponentThrottleVector * opponentThrottle * 0.038 * g_FPSFactor)
          6:
            opponentVelVector = opponentVelVector - (opponentThrottleVector * opponentThrottle * 0.04 * g_FPSFactor)
        end case
      end if
      tempFactor = -mOpponentCar.transform.zAxis.dot(opponentVelVector)
      opponentSteerVel = opponentSteer * 0.001 * tempFactor
      if opponentSteerVel > 0.5 then
        opponentSteerVel = 0.5
      end if
      if opponentSteerVel < -0.5 then
        opponentSteerVel = -0.5
      end if
      if (opponentImbedList[1] > 0) or (opponentImbedList[2] > 0) then
        opponentRotVector.y = opponentRotVector.y - (opponentSteerVel * g_FPSFactor)
      end if
      if opponentMidAir = 1 then
        opponentVelVector.y = opponentVelVector.y - (1.0 * g_FPSFactor)
      else
        opponentVelVector.y = opponentVelVector.y - (1.0 * g_FPSFactor)
      end if
      if midAir = 0 then
        repeat with i = g_FPSFactorDec down to 0
          if i = 0 then
            opponentVelVector.y = opponentVelVector.y - (opponentVelVector.y * 0.05 * g_FPSFactorFrac)
            opponentVelVector = opponentVelVector - (opponentVelVector * 0.005 * g_FPSFactorFrac)
            opponentRotVector = opponentRotVector - (opponentRotVector * 0.16 * g_FPSFactorFrac)
            next repeat
          end if
          opponentVelVector.y = opponentVelVector.y - (opponentVelVector.y * 0.05)
          opponentVelVector = opponentVelVector - (opponentVelVector * 0.005)
          opponentRotVector = opponentRotVector - (opponentRotVector * 0.16)
        end repeat
      end if
      tempVector = opponentVelVector.duplicate()
      tempFactor = tempVector.dot(mOpponentCar.transform.xAxis)
      opponentGripVector = mOpponentCar.transform.xAxis
      opponentGripVector = opponentGripVector * tempFactor
      if opponentGripVector.magnitude > (1.0 * g_FPSFactor) then
        opponentGripVector.normalize()
        opponentGripVector = opponentGripVector * 1.0 * g_FPSFactor
        tempFactor = mOpponentCar.transform.xAxis.dot(opponentVelVector)
      end if
      if opponentMidAir = 0 then
        opponentVelVector = opponentVelVector - opponentGripVector
      end if
      tempVector = opponentVelVector - prevOpponentVelVector
      tempVector.y = 0
      if tempVector.magnitude > 1 then
        tempVector = tempVector.getNormalized() * 1
      end if
      opponentRotVector.x = opponentRotVector.x - (tempVector.z * 0.14999999999999999 * g_FPSFactor)
      opponentRotVector.z = opponentRotVector.z + (tempVector.x * 0.14999999999999999 * g_FPSFactor)
      gOpponentCar.translate((opponentVelVector * g_FPSFactor) + vector(0, opponentHoist * g_FPSFactor, 0), #world)
      mOpponentCar.rotate(opponentRotVector * g_FPSFactor, #parent)
      opponentRollVector = opponentVelVector.duplicate() * 1.60000000000000009
      opponentRollVector.y = 0
      opponentRollFactor = opponentRollVector.magnitude
      opponentRollVector.normalize()
      opponentRollVel = opponentRollVector.dot(mOpponentCar.transform.zAxis) * opponentRollFactor * g_FPSFactor
      mOpponentTireFL.rotate(opponentRollVel + (opponentThrottle * 2), 0, 0, #self)
      mOpponentTireFR.rotate(opponentRollVel + (opponentThrottle * 2), 0, 0, #self)
      mOpponentTireBL.rotate(opponentRollVel + (opponentThrottle * 2), 0, 0, #self)
      mOpponentTireBR.rotate(opponentRollVel + (opponentThrottle * 2), 0, 0, #self)
      mOpponentTireFL.transform.rotation.y = opponentSteer
      mOpponentTireFR.transform.rotation.y = -opponentSteer
      mOpponentTireFL.transform.position.y = 16 + opponentImbedList[4]
      mOpponentTireBL.transform.position.y = 16 + opponentImbedList[2]
      mOpponentTireFR.transform.position.y = 16 + opponentImbedList[3]
      mOpponentTireBR.transform.position.y = 16 + opponentImbedList[1]
  end case
  tempRadius = 160
  diffVector = gOpponentCar.worldPosition + (opponentVelVector * g_FPSFactor) - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
  if diffVector.magnitude < tempRadius then
    pushVector = diffVector.getNormalized() * (tempRadius - diffVector.magnitude)
    velVector = velVector - (pushVector / g_FPSFactor)
    rotVector.z = rotVector.z - (pushVector.x * 0.20000000000000001 / g_FPSFactor)
    rotVector.x = rotVector.x + (pushVector.z * 0.20000000000000001 / g_FPSFactor)
    pushMag = pushVector.magnitude / g_FPSFactor
    if pushMag > 5 then
      g_DamagePercent = g_DamagePercent + 0.0005
      if g_Muted = 0 then
        puppetSound(1, member(16, "Sfx"))
      end if
    end if
    if pushMag > 15 then
      g_DamagePercent = g_DamagePercent + 0.001
      if g_Muted = 0 then
        puppetSound(1, member(18, "Sfx"))
      end if
    end if
  end if
end

on doSetupTraffic
  trafficDotIDList = []
  trafficPrevDotIDList = []
  mTrafficList = []
  trafficTargetPosList = []
  trafficSpeedList = []
  carTargetSpeedList = []
  dotTrafficIDList = []
  trafficStateList = []
  dotOccupiedList = []
  dotOccupiedList[dotPosList.count] = 0
  trafficDotSpawnTicksList = []
  trafficDotSpawnTicksList[dotPosList.count] = 0
  trafficSpawnedCounter = 0
  trafficSpawnedLimit = 5
  repeat with i = 1 to 20
    tempGroup = w.newGroup("TraficCarGroup" & i)
    tempGroup.addChild(mTrafficCar.clone("TrafficCarClone" && mTrafficList.count))
    tempGroup.worldPosition = vector(-1000, -1000, -1000)
    mTrafficList.add(tempGroup)
    trafficStateList.add(0)
    trafficSpeedList.add(0)
    trafficTargetSpeedList.add(0)
    trafficTargetPosList.add(vector(0, 0, 0))
    trafficDotIDList.add(0)
    trafficPrevDotIDList.add(0)
    dotTrafficIDList.add(0)
  end repeat
end

on doTraffic
  trafficSpawnDotIDList = []
  tempTileID = onTile
  tempTileList = [tempTileID]
  tempTileXID = ((tempTileID - 1) mod tileCols) + 1
  tempTileZID = ((tempTileID - 1 - ((tempTileID - 1) mod tileCols)) / tileCols) + 1
  if ((tempTileXID - 1) > 0) and ((tempTileZID - 1) > 0) then
    tempTileList.add(tempTileID - tileCols - 1)
  end if
  if (tempTileZID - 1) > 0 then
    tempTileList.add(tempTileID - tileCols)
  end if
  if ((tempTileZID - 1) > 0) and ((tempTileXID + 1) <= tileCols) then
    tempTileList.add(tempTileID - tileCols + 1)
  end if
  if (tempTileXID - 1) > 0 then
    tempTileList.add(tempTileID - 1)
  end if
  if (tempTileXID + 1) <= tileCols then
    tempTileList.add(tempTileID + 1)
  end if
  if ((tempTileXID - 1) > 0) and ((tempTileZID + 1) <= tileRows) then
    tempTileList.add(tempTileID + tileCols - 1)
  end if
  if (tempTileZID + 1) <= tileCols then
    tempTileList.add(tempTileID + tileCols)
  end if
  if ((tempTileXID + 1) <= tileCols) and ((tempTileZID + 1) <= tileRows) then
    tempTileList.add(tempTileID + tileCols + 1)
  end if
  tempiCount = tempTileList.count
  repeat with i = 1 to tempiCount
    tempjCount = tileDotIDList[tempTileList[i]].count
    repeat with j = 1 to tempjCount
      tempDotID = tileDotIDList[tempTileList[i]][j]
      diffVector = mPlayerCar.worldPosition - dotPosList[tempDotID]
      diffVector.y = 0
      tempMag = diffVector.magnitude
      if (tempMag < trafficCullDist) and (tempMag > trafficSpawnDist) then
        if (dotOccupiedList[tempDotID] = 0) and ((the ticks - trafficDotSpawnTicksList[tempDotID]) > 200) then
          trafficSpawnDotIDList.add(tempDotID)
        end if
        next repeat
      end if
    end repeat
  end repeat
  tempiCount = mTrafficList.count
  repeat with i = 1 to tempiCount
    case trafficStateList[i] of
      0:
        if trafficSpawnDotIDList.count > 0 then
          if trafficSpawnedCounter < trafficSpawnedLimit then
            trafficStateList[i] = 1
            mTrafficList[i].child[1].transform.rotation = vector(0, 180, 0)
            mTrafficList[i].transform.rotation = vector(0, 0, 0)
            tempRand = random(trafficSpawnDotIDList.count)
            tempDotID = trafficSpawnDotIDList[tempRand]
            mTrafficList[i].worldPosition = dotPosList[tempDotID]
            trafficTargetPosList[i] = mTrafficList[i].worldPosition
            trafficDotIDList[i] = tempDotID
            dotOccupiedList[tempDotID] = i
            trafficSpawnDotIDList.deleteAt(tempRand)
            trafficSpawnedCounter = trafficSpawnedCounter + 1
            trafficDotSpawnTicksList[tempDotID] = the ticks
            if nextDotIDList[tempDotID].count > 0 then
              tempNextDotID = nextDotIDList[tempDotID][1]
              tempPos = dotPosList[tempNextDotID].duplicate()
              tempPos.y = mTrafficList[i].worldPosition.y
              if mTrafficList[i].worldPosition <> tempPos then
                mTrafficList[i].pointAt(tempPos)
              end if
              tempSpeed = (trafficTargetPosList[i] - dotPosList[tempNextDotID]).magnitude * 0.02
              trafficTargetSpeedList[i] = tempSpeed
            else
              trafficTargetSpeedList[i] = 0
              trafficSpeedList[i] = 0
              if prevDotIDList[tempDotID].count > 0 then
                tempPrevDotID = prevDotIDList[tempDotID][1]
                tempPos = dotPosList[tempPrevDotID]
                tempPos.y = mTrafficList[i].worldPosition.y
                if mTrafficList[i].worldPosition <> tempPos then
                  mTrafficList[i].pointAt(tempPos)
                end if
              end if
            end if
            mTrafficList[i].addToWorld()
          end if
        end if
      1:
        trafficSpeedList[i] = trafficSpeedList[i] + ((trafficTargetSpeedList[i] - trafficSpeedList[i]) * 0.04)
        tempTraffic = mTrafficList[i]
        tempPrevPos = tempTraffic.worldPosition
        tempVector = tempTraffic.worldPosition - trafficTargetPosList[i]
        tempVector.y = 0
        tempDist = tempVector.magnitude
        if tempDist < 100 then
          tempDotCount = nextDotIDList[trafficDotIDList[i]].count
          if tempDotCount > 0 then
            tempRand = random(tempDotCount)
            tempNextDotID = nextDotIDList[trafficDotIDList[i]][tempRand]
            if dotOccupiedList[tempNextDotID] = 0 then
              if abs(dotOccupiedList[trafficDotIDList[i]]) = i then
                if dotOccupiedList[trafficDotIDList[i]] < 0 then
                  dotOccupiedList[trafficDotIDList[i]] = -1111
                else
                  dotOccupiedList[trafficDotIDList[i]] = 0
                end if
              end if
              dotOccupiedList[tempNextDotID] = i
              tempSpeed = (trafficTargetPosList[i] - dotPosList[tempNextDotID]).magnitude * 0.02
              trafficTargetSpeedList[i] = tempSpeed
              trafficDotSpawnTicksList[trafficDotIDList[i]] = the ticks
              trafficDotIDList[i] = tempNextDotID
              trafficTargetPosList[i] = dotPosList[trafficDotIDList[i]]
            else
              trafficTargetSpeedList[i] = 0
              trafficSpeedList[i] = 0
            end if
          else
            trafficTargetSpeedList[i] = 0
            trafficSpeedList[i] = 0
          end if
        else
          tempVector.normalize()
          tempRot = tempVector.dot(mTrafficList[i].transform.xAxis) * 4.5 * g_FPSFactor
          tempTraffic.rotate(0, tempRot, 0)
          tempTraffic.translate(0, 0, -trafficSpeedList[i] * g_FPSFactor, #self)
          tempCol = integer((tempTraffic.worldPosition.x / 5000.0) + 0.5)
          tempRow = integer((tempTraffic.worldPosition.z / 5000.0) + 0.5)
          tempTile = tempCol + ((tempRow - 1) * tileCols)
          tempTileID = tileIDList[tempTile]
          tempModelList = [mTileResourceList[tempTileID]]
          tempOptionsList = [#modelList: tempModelList, #levelOfDetail: #detailed]
          mTileResourceList[tempTileID].addToWorld()
          mTileResourceList[tempTileID].transform.rotation = vector(0, -90 * tileRotList[tempTile], 0)
          tempRayOrigin = vector(tempTraffic.worldPosition.x mod 5000, tempTraffic.worldPosition.y + 100, tempTraffic.worldPosition.z mod 5000)
          tempResult = w.modelsUnderRay(tempRayOrigin, vector(0, -1, 0), tempOptionsList)
          mTileResourceList[tempTileID].removeFromWorld()
          if tempResult.count > 0 then
            tempTraffic.worldPosition.y = tempResult[1].isectPosition.y
          end if
        end if
        if tempPrevPos <> tempTraffic.worldPosition then
          tempTraffic.child[1].pointAt(tempPrevPos)
        end if
        diffVector = mTrafficList[i].worldPosition - gPlayerCar.worldPosition
        diffVector.y = 0
        tempMag = diffVector.magnitude
        if tempMag > trafficCullDist then
          mTrafficList[i].removeFromWorld()
          trafficStateList[i] = 0
          dotOccupiedList[trafficDotIDList[i]] = 0
          trafficDotIDList[i] = 0
          trafficSpawnedCounter = trafficSpawnedCounter - 1
        end if
    end case
  end repeat
end

on doLoadData
  put "loadStart"
  tempLoadList = value(member(1, "MapData").text)
  tempDotPosList = tempLoadList[1]
  nextDotIDList = tempLoadList[2]
  prevDotIDList = tempLoadList[3]
  redTrafficLightIDList = tempLoadList[4]
  greenTrafficLightIDList = tempLoadList[5]
  tileDotIDList = tempLoadList[6]
  put "loadStringsComplete"
  tempiCount = tempDotPosList.count
  repeat with i = 1 to tempiCount
    tempVector = vector(tempDotPosList[i][1] * 10, tempDotPosList[i][2] * 10, tempDotPosList[i][3] * 10)
    dotPosList.add(tempVector)
  end repeat
  put "ConvertDots Complete"
  redDotPosList = [[], [], [], [], []]
  racePodPosList = [[], [], [], [], []]
  put "Red LoadStart"
  repeat with i = 1 to 5
    put "FiledOn" && i
    tempLoadList = value(member(i + 1, "MapData").text)
    tempRedDotPosList = tempLoadList[1]
    tempRedDotCheckPointList = tempLoadList[2]
    tempiCount = tempRedDotPosList.count
    repeat with j = 1 to tempiCount
      tempVector = vector(tempRedDotPosList[j][1] * 10, tempRedDotPosList[j][2] * 10, tempRedDotPosList[j][3] * 10)
      redDotPosList[i].add(tempVector)
    end repeat
    repeat with j = 1 to tempRedDotCheckPointList.count
      racePodPosList[i].add(redDotPosList[i][tempRedDotCheckPointList[j]])
    end repeat
  end repeat
  put "Red LoadStringsComplete"
end

on doSetupPlayerCar
  g_DamagePercent = g_CarsDamageList[playerCarID]
  g_BoostPercent = g_CarsIgNitroList[playerCarID]
  g_CurrentScore = 0
  g_GameOverID = 0
  damageStartTicks = 0
  soundMemberList = [VOID, VOID, VOID, VOID, VOID, VOID]
  camState = 1
  cam = sprite(1).camera
  cam.fieldOfView = 55
  camRange = 200
  camHeight = 50
  gPlayerCar.worldPosition = vector(32053.56999999999970896, -5.55999999999999961, 30797.50999999999839929)
  mPlayerCar.transform.rotation = vector(0, 45, 0)
  camAim = gPlayerCar.worldPosition + vector(-80, 50, 200)
  cam.worldPosition = gPlayerCar.worldPosition + vector(-80, 50, -200)
  carDamageList = [0, 0, 0, 0, 0, 0]
  carBoostList = [1, 1, 1, 1, 1, 1]
  carColorIDList = [1, 1, 1, 1, 1, 1]
  gCageList = []
  isectPosList = []
  isectPosList[12] = 0
  isectNormList = []
  isectNormList[12] = 0
  imbedList = []
  imbedList[12] = 0
  imbedIDList = []
  imbedIDList[12] = 1
  imbedCapList = []
  imbedCapList[12] = 0
  imbedRotList = []
  imbedRotList[12] = 0
  toCenterList = []
  toCenterList[12] = vector(0, 0, 0)
  gripVector = vector(0, 0, 0)
  throttle = 0
  prevBoosting = 0
  midAir = 0
  hoist = 0
  hoistCap = 0
  gPlayerCage = w.newGroup("CageGroup")
  mPlayerCar.addChild(gPlayerCage)
  gPlayerCage.transform.position = vector(0, 0, 0)
  repeat with i = 1 to 12
    gCageList.add(w.newGroup("cagePointGroup" & i))
    gPlayerCage.addChild(gCageList[i])
  end repeat
  gCageList[1].transform.position = vector(-50, 30, -60)
  gCageList[2].transform.position = vector(50, 30, -60)
  gCageList[3].transform.position = vector(-50, 30, 60)
  gCageList[4].transform.position = vector(50, 30, 60)
  gCageList[5].transform.position = vector(-55, 40, -60)
  gCageList[6].transform.position = vector(55, 40, -60)
  gCageList[7].transform.position = vector(-55, 45, 60)
  gCageList[8].transform.position = vector(55, 45, 60)
  gCageList[9].transform.position = vector(-50, 90, -10)
  gCageList[10].transform.position = vector(50, 90, -10)
  gCageList[11].transform.position = vector(-50, 100, 45)
  gCageList[12].transform.position = vector(50, 100, 45)
  mIgNitroBlastL = w.model("igNitroBlastJet_000")
  mIgNitroBlastR = mIgNitroBlastL.clone("IgNitroBlastJetClone")
  mIgNitroBlastL.shaderList[1].texture.renderFormat = #rgba8888
  mIgNitroBlastL.shaderList[2].texture.renderFormat = #rgba8888
  mPlayerCar.addChild(mIgNitroBlastL)
  mPlayerCar.addChild(mIgNitroBlastR)
  mPlayerCar.addChild(gIgNitro)
  gIgNitro.addChild(mIgNitro)
  mIgNitroBlastL.transform.rotation = vector(10, 0, 0)
  mIgNitroBlastR.transform.rotation = vector(10, 0, 0)
  case playerCarID of
    1:
      mIgNitroBlastL.transform.position = vector(30, 20, -95)
      mIgNitroBlastL.transform.position = vector(30, 20, -95)
      gIgNitro.transform.position = vector(0, 85, -80)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
    2:
      mIgNitroBlastL.transform.position = vector(-30, 25, -125)
      mIgNitroBlastR.transform.position = vector(30, 25, -125)
      gIgNitro.transform.position = vector(0, 96, -110)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
    3:
      mIgNitroBlastL.transform.position = vector(-38, 25, -125)
      mIgNitroBlastR.transform.position = vector(38, 25, -125)
      gIgNitro.transform.position = vector(0, 92, -110)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
    4:
      mIgNitroBlastL.transform.position = vector(-30, 22, -120)
      mIgNitroBlastR.transform.position = vector(30, 22, -120)
      gIgNitro.transform.position = vector(0, 94, -95)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
    5:
      mIgNitroBlastL.transform.position = vector(-36, 20, -140)
      mIgNitroBlastR.transform.position = vector(36, 20, -140)
      gIgNitro.transform.position = vector(0, 92, -120)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
    6:
      mIgNitroBlastL.transform.position = vector(-45, 25, -120)
      mIgNitroBlastR.transform.position = vector(45, 25, -120)
      gIgNitro.transform.position = vector(0, 105, -70)
      gIgNitro.transform.rotation = vector(0, 0, 0)
      mIgNitro.transform.position = vector(0, -50, 0)
      mIgNitro.transform.rotation = vector(0, 0, 0)
  end case
  tempRot = mPlayerCar.transform.rotation.duplicate()
  mPlayerCar.removeFromWorld()
  mPlayerCar = mShellList[playerCarID]
  gPlayerCar.addChild(mPlayerCar)
  mPlayerCar.transform.position = vector(0, 0, 0)
  mPlayerCar.transform.rotation = tempRot
  mPlayerCar.addToWorld()
  mPlayerCar.addChild(gPlayerCage)
  gPlayerCage.transform.position = vector(0, 0, 0)
  gPlayerCage.transform.rotation = vector(0, 0, 0)
  gCarPlaceHolder = w.newGroup("CarPlaceHolderGroup")
  gTirePlaceHolder = w.newGroup("TirePlaceHolderGroup")
  gCarPlaceHolder.addChild(gTirePlaceHolder)
end

on doSetupObstacleDust
  tempTexture = w.newTexture("ObstacleDustTexture", #fromCastMember, member(7, "Textures"))
  tempTexture.renderFormat = #rgba8888
  tempTexture.quality = #low
  nmr = w.newModelResource("obstacleDustModelResource", #particle)
  mObstacleDust = w.newModel("obstacleDustModel", nmr)
  nmr.texture = tempTexture
  nmr.wind = vector(0, 0, 0)
  nmr.gravity = vector(0, -10, 0)
  nmr.drag = 2
  nmr.blendRange.start = 100
  nmr.blendRange.end = 0
  nmr.sizeRange.start = 10
  nmr.sizeRange.end = 50
  nmr.colorRange.start = rgb(255, 255, 255)
  nmr.colorRange.end = rgb(255, 255, 255)
  nmr.lifeTime = 1000
  nmr.emitter.tweenMode = #age
  nmr.emitter.angle = 20
  nmr.emitter.minSpeed = 1000
  nmr.emitter.maxSpeed = 1600
  nmr.emitter.numParticles = 128
  nmr.emitter.region = [vector(0, 1000000, 0)]
  nmr.emitter.direction = vector(0, 1, 0)
end

on doSetupBurnSmoke
  tempTexture = w.newTexture("BurnSmokeTexture", #fromCastMember, member(6, "Textures"))
  tempTexture.renderFormat = #rgba8888
  tempTexture.quality = #low
  nmr = w.newModelResource("burnSmokeModelResource", #particle)
  mBurnSmoke = w.newModel("burnSmokeModel", nmr)
  nmrBurnSmoke = nmr
  nmr.texture = tempTexture
  nmr.wind = vector(0, 0, 0)
  nmr.gravity = vector(0, -1, 0)
  nmr.drag = 1
  nmr.blendRange.start = 100
  nmr.blendRange.end = 0
  nmr.sizeRange.start = 40
  nmr.sizeRange.end = 50
  nmr.colorRange.start = rgb(255, 255, 255)
  nmr.colorRange.end = rgb(100, 100, 0)
  nmr.lifeTime = 1000
  nmr.emitter.tweenMode = #age
  nmr.emitter.angle = 20
  nmr.emitter.minSpeed = 300
  nmr.emitter.maxSpeed = 360
  nmr.emitter.numParticles = 64
  nmr.emitter.region = [vector(0, 10000000, 0)]
  nmr.emitter.direction = vector(0, 1, 0)
  tireSmokeToggle = 1
  tempTexture = w.newTexture("TireSmokeTexture", #fromCastMember, member(19, "Textures"))
  tempTexture.renderFormat = #rgba8888
  tempTexture.quality = #low
  nmr = w.newModelResource("tireSmokeModelResource", #particle)
  mTireSmoke = w.newModel("tireSmokeModel", nmr)
  nmrTireSmoke = nmr
  nmr.texture = tempTexture
  nmr.wind = vector(0, 0, 0)
  nmr.gravity = vector(0, -2, 0)
  nmr.drag = 1
  nmr.blendRange.start = 100
  nmr.blendRange.end = 0
  nmr.sizeRange.start = 20
  nmr.sizeRange.end = 50
  nmr.colorRange.start = rgb(255, 255, 255)
  nmr.colorRange.end = rgb(100, 100, 100)
  nmr.lifeTime = 200
  nmr.emitter.tweenMode = #age
  nmr.emitter.angle = 90
  nmr.emitter.minSpeed = 200
  nmr.emitter.maxSpeed = 260
  nmr.emitter.numParticles = 32
  nmr.emitter.region = [vector(0, 10000000, 0)]
  nmr.emitter.direction = vector(0, 1, 0)
end

on doTireGrip
  tempTireSmoke = 0
  tempVector = velVector.duplicate()
  tempFactor = tempVector.dot(mPlayerCar.getWorldTransform().xAxis)
  gripVector = mPlayerCar.getWorldTransform().xAxis
  gripVector = gripVector * tempFactor
  if gripVector.magnitude > (1.0 * g_FPSFactor) then
    gripVector.normalize()
    gripVector = gripVector * 1.0 * g_FPSFactor
    tempFactor = mPlayerCar.getWorldTransform().xAxis.dot(velVector)
    if g_Muted = 0 then
      if tempFactor > 8 then
        tempTireSmoke = 1
        if soundMemberList[4] <> member(43, "Sfx") then
          sound(4).play(member(43, "Sfx"))
          soundMemberList[4] = member(43, "Sfx")
        end if
      end if
      if tempFactor < -8 then
        tempTireSmoke = 1
        if soundMemberList[4] <> member(42, "Sfx") then
          sound(4).play(member(42, "Sfx"))
          soundMemberList[4] = member(42, "Sfx")
        end if
      end if
    end if
  end if
  if midAir = 0 then
    velVector = velVector - gripVector
  end if
  if tempTireSmoke = 1 then
    tireSmokeToggle = -tireSmokeToggle
    if tireSmokeToggle > 0 then
      if tempFactor < 0 then
        tireSmokeTire = 4
      else
        tireSmokeTire = 3
      end if
    else
      if tempFactor < 0 then
        tireSmokeTire = 1
      else
        tireSmokeTire = 2
      end if
    end if
    nmrTireSmoke.emitter.region = [gCageList[tireSmokeTire].worldPosition + vector(0, -15, 0)]
  else
    nmrTireSmoke.emitter.region = [vector(0, 100000, 0)]
  end if
end

on doBodyRoll
  tempVector = velVector - prevVelVector
  tempVector.y = 0
  if tempVector.magnitude > 1 then
    tempVector = tempVector.getNormalized() * 1
  end if
  rotVector.x = rotVector.x - (tempVector.z * 0.34999999999999998 * g_FPSFactor)
  rotVector.z = rotVector.z + (tempVector.x * 0.34999999999999998 * g_FPSFactor)
end

on doSetUpTiles
  put w.model("TILE_DockCanalObjects_001").shaderList
  tCanalWater = w.model("TILE_DockCanalObjects_001").shaderList[2]
  gVisTiles = w.newGroup("VisTilesGroup")
  mSkyBox = w.model(6)
  mSkyBox.shader.emissive = rgb(255, 255, 255)
  mSkyBox.shader.diffuse = rgb(0, 0, 0)
  mSkyBox.shader.texture.renderFormat = #rgba8880
  w.model("TILE_NurdBurgerRing_StraightObjects_000").shaderList[1].texture.renderFormat = #rgba4444
  mTileResourceList = []
  tempCount = mSkyBox.child.count
  repeat with i = 1 to tempCount
    mTileResourceList.add(mSkyBox.child[i])
  end repeat
  repeat with i = 1 to tempCount
    mTileResourceList[i].removeFromWorld()
  end repeat
  shadowTexture = w.newTexture("shadowTexture", #fromCastMember, member(4, "Textures"))
  shadowTexture.renderFormat = #rgba8888
  shadowTexture.quality = #low
  onTileRow = integer((gPlayerCar.worldPosition.z / 5000.0) + 0.5)
  onTileCol = integer((gPlayerCar.worldPosition.x / 5000.0) + 0.5)
  onTile = onTileCol + ((onTileRow - 1) * tileCols)
  onTileX = gPlayerCar.worldPosition.x mod 5000
  onTilez = gPlayerCar.worldPosition.z mod 5000
  minVisCol = onTileCol - visTileDist
  maxVisCol = onTileCol + visTileDist
  minVisRow = onTileRow - visTileDist
  maxVisRow = onTileRow + visTileDist
  if minVisCol < 1 then
    minVisCol = 1
  end if
  if maxVisCol > tileCols then
    maxVisCol = tileCols
  end if
  if minVisRow < 1 then
    minVisRow = 1
  end if
  if maxVisRow > tileRows then
    maxVisRow = tileRows
  end if
  tempRep = (1 + (visTileDist * 2)) * (1 + (visTileDist * 2))
  repeat with i = 1 to visTileRows
    repeat with j = 1 to visTileCols
      tempTile = w.newModel("TileModel" & j + (i * visTileCols))
      tempTile.resource = mTileResourceList[1].resource
      tempTile.shaderList = mTileResourceList[1].shaderList
      tempObjectTile = w.newModel("TileObjectsModel" & j + (i * visTileCols))
      tempObjectTile.resource = mTileResourceList[1].child[1].resource
      tempObjectTile.shaderList = mTileResourceList[1].child[1].shaderList
      mVisTileList.add(tempTile)
      if (i >= visTileDist) and (i <= (visTileDist + 2)) and (j >= visTileDist) and (j <= (visTileDist + 2)) then
        tempShadowTile = tempTile.cloneDeep("TileShadowModel" & j + (i * visTileCols))
        tempShadowTile.shaderList[1].texture = shadowTexture
        tempShadowTile.shaderList[1].textureRepeat = 0
        if shadowsOn = 0 then
          tempShadowTile.visibility = #none
        end if
      end if
      tempTile.addChild(tempObjectTile)
      if (i >= visTileDist) and (i <= (visTileDist + 2)) and (j >= visTileDist) and (j <= (visTileDist + 2)) then
        tempTile.addChild(tempShadowTile)
      end if
      gVisTiles.addChild(tempTile)
      tempTile.worldPosition = vector(((j - 1) * 5000) - (5000 * visTileDist), 0, ((i - 1) * 5000) - (5000 * visTileDist)) + vector(2500, 0, 2500)
    end repeat
  end repeat
  gVisTiles.worldPosition = vector((onTileCol - 1) * 5000, 0, (onTileRow - 1) * 5000)
  doTiles()
end

on doTiles
  repeat with i = 1 to 12
    tileColList[i] = integer((gCageList[i].worldPosition.x / 5000.0) + 0.5)
    tileRowList[i] = integer((gCageList[i].worldPosition.z / 5000.0) + 0.5)
    imbedIDList[i] = tileIDList[tileColList[i] + ((tileRowList[i] - 1) * tileCols)]
    imbedRotList[i] = tileRotList[tileColList[i] + ((tileRowList[i] - 1) * tileCols)]
  end repeat
  onTileRow = integer((gPlayerCar.worldPosition.z / 5000.0) + 0.5)
  onTileCol = integer((gPlayerCar.worldPosition.x / 5000.0) + 0.5)
  onTile = onTileCol + ((onTileRow - 1) * tileCols)
  onTileID = tileIDList[onTile]
  onTileX = gPlayerCar.worldPosition.x mod 5000
  onTilez = gPlayerCar.worldPosition.z mod 5000
  minVisCol = onTileCol - visTileDist
  maxVisCol = onTileCol + visTileDist
  minVisRow = onTileRow - visTileDist
  maxVisRow = onTileRow + visTileDist
  if onTile <> prevOnTile then
    gVisTiles.worldPosition = vector((onTileCol - 1) * 5000, 0, (onTileRow - 1) * 5000)
    repeat with i = 1 to visTileRows
      repeat with j = 1 to visTileCols
        currentCol = minVisCol + j - 1
        currentRow = minVisRow + i - 1
        currentTile = 0
        if (currentCol > 0) and (currentCol <= tileCols) then
          if (currentRow > 0) and (currentRow <= tileRows) then
            currentTile = currentCol + ((currentRow - 1) * tileCols)
          end if
        end if
        tempTile = j + ((i - 1) * visTileCols)
        if currentTile = 0 then
          visTileIDList[tempTile] = 0
          mVisTileList[tempTile].resource = mTileResourceList[1].resource
          mVisTileList[tempTile].shaderList = mTileResourceList[1].shaderList
          if mVisTileList[tempTile].child.count > 1 then
            mVisTileList[tempTile].child[2].resource = mTileResourceList[1].child[2].resource
          end if
          mVisTileList[tempTile].child[1].resource = mTileResourceList[1].child[1].resource
          mVisTileList[tempTile].child[1].shaderList = mTileResourceList[1].child[1].shaderList
          mVisTileList[tempTile].transform.rotation = vector(0, 0, 0)
          next repeat
        end if
        mVisTileList[tempTile].resource = mTileResourceList[tileIDList[currentTile]].resource
        mVisTileList[tempTile].shaderList = mTileResourceList[tileIDList[currentTile]].shaderList
        if mVisTileList[tempTile].child.count > 1 then
          mVisTileList[tempTile].child[2].resource = mTileResourceList[tileIDList[currentTile]].child[2].resource
        end if
        mVisTileList[tempTile].child[1].resource = mTileResourceList[tileIDList[currentTile]].child[1].resource
        mVisTileList[tempTile].child[1].shaderList = mTileResourceList[tileIDList[currentTile]].child[1].shaderList
        visTileIDList[tempTile] = tileIDList[currentTile]
        mVisTileList[tempTile].transform.rotation = vector(0, -90 * tileRotList[currentTile], 0)
        visTileRotList[tempTile] = tileRotList[currentTile]
      end repeat
    end repeat
  end if
  if shadowsOn = 1 then
    repeat with i = 1 to visTileRows
      repeat with j = 1 to visTileCols
        if (i >= visTileDist) and (i <= (visTileDist + 2)) and (j >= visTileDist) and (j <= (visTileDist + 2)) then
          tempVisTile = j + ((i - 1) * visTileCols)
          tempRot = visTileRotList[tempVisTile]
          tempOffsetVector = (gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempVisTile].worldPosition) * 0.004
          tempOffsetVector.y = 0
          offsetVector = vector(0, 0, 0)
          case tempRot of
            0:
              offsetVector.x = tempOffsetVector.x
              offsetVector.y = tempOffsetVector.z
            1:
              offsetVector.x = tempOffsetVector.z
              offsetVector.y = -tempOffsetVector.x
            2:
              offsetVector.x = -tempOffsetVector.x
              offsetVector.y = -tempOffsetVector.z
            3:
              offsetVector.x = -tempOffsetVector.z
              offsetVector.y = tempOffsetVector.x
          end case
          mVisTileList[tempVisTile].child[2].shaderList[1].textureTransformList[1].position = offsetVector
          if getRendererServices().renderer = #software then
            mVisTileList[j + ((i - 1) * visTileCols)].child[2].visibility = #none
            shadowsOn = 0
          end if
        end if
      end repeat
    end repeat
  else
    if getRendererServices().renderer <> #software then
      repeat with i = 1 to visTileRows
        repeat with j = 1 to visTileCols
          if (i >= visTileDist) and (i <= (visTileDist + 2)) and (j >= visTileDist) and (j <= (visTileDist + 2)) then
            mVisTileList[j + ((i - 1) * visTileCols)].child[2].visibility = #front
          end if
          shadowsOn = 1
        end repeat
      end repeat
    end if
  end if
  mTileResourceList[2].child[1].shaderList[1].textureTransform.translate(0, -0.02, 0)
end

on doControls
  keyPressedList = [keyPressed(123), keyPressed(124), keyPressed(125), keyPressed(126), keyPressed(49)]
  if keyPressedList[1] = 1 then
    if steer < 0 then
      steer = 0
    end if
    if steer < 40 then
      steer = steer + (2 * g_FPSFactor)
    end if
  end if
  if keyPressedList[2] = 1 then
    if steer > 0 then
      steer = 0
    end if
    if steer > -40 then
      steer = steer - (2 * g_FPSFactor)
    end if
  end if
  if (keyPressedList[1] = 0) and (keyPressedList[2] = 0) then
    if steer > 0 then
      if steer >= (5 * g_FPSFactor) then
        steer = steer - (5 * g_FPSFactor)
      else
        steer = 0
      end if
    end if
    if steer < 0 then
      if steer <= (-5 * g_FPSFactor) then
        steer = steer + (5 * g_FPSFactor)
      else
        steer = 0
      end if
    end if
  end if
  if (keyPressedList[4] = 1) and (keyPressedList[3] = 0) then
    if midAir = 0 then
      if throttle < 0 then
        throttle = 0
      end if
    end if
    if g_DamagePercent < 1.0 then
      throttle = throttle + (0.20000000000000001 * g_FPSFactor)
    end if
  end if
  if (keyPressedList[3] = 1) and (keyPressedList[4] = 0) then
    if midAir = 0 then
      if throttle > 0 then
        throttle = 0
      end if
    end if
    if g_DamagePercent < 1.0 then
      throttle = throttle - (0.20000000000000001 * g_FPSFactor)
    end if
    if rollVel > 0 then
      if g_Muted = 0 then
        if soundMemberList[2] <> member(14, "Sfx") then
          puppetSound(2, member(14, "Sfx"))
          soundMemberList[2] = member(14, "Sfx")
        end if
      end if
    else
      if g_Muted = 0 then
        if soundMemberList[2] <> member(13, "Sfx") then
          puppetSound(2, member(13, "Sfx"))
          soundMemberList[2] = member(13, "Sfx")
        end if
      end if
    end if
  end if
  if (keyPressedList[3] = 0) and (keyPressedList[4] = 0) then
    throttle = 0
    tempVector = velVector.duplicate()
    tempVector.y = 0
  end if
  if throttle < -8 then
    throttle = -8
  end if
  if throttle > 10 then
    throttle = 10
  end if
  if (midAir = 0) and (prevMidAir = 1) then
    if velVector.y < -10 then
      if g_Muted = 0 then
        if soundBusy(1) = 0 then
          puppetSound(1, member(21, "Sfx"))
          soundMemberList[1] = member(21, "Sfx")
        end if
      end if
      if velVector.y < -20 then
        if g_Muted = 0 then
          puppetSound(1, member(20, "Sfx"))
          soundMemberList[1] = member(20, "Sfx")
        end if
      end if
    end if
  end if
  if sound(1).member = VOID then
    tempVector = rotVector.duplicate()
    tempVector.y = 0
    if tempVector.magnitude > 5 then
      if g_Muted = 0 then
        if soundBusy(1) = 0 then
          puppetSound(1, member(21, "Sfx"))
          soundMemberList[1] = member(21, "Sfx")
        end if
      end if
    end if
  end if
  throttleVector = -mPlayerCar.transform.zAxis
  if midAir = 0 then
    if g_DamagePercent < 1 then
      case playerCarID of
        1:
          velVector = velVector - (throttleVector * throttle * 0.025 * g_FPSFactor)
        2:
          velVector = velVector - (throttleVector * throttle * 0.0325 * g_FPSFactor)
        3:
          velVector = velVector - (throttleVector * throttle * 0.035 * g_FPSFactor)
        4:
          velVector = velVector - (throttleVector * throttle * 0.0375 * g_FPSFactor)
        5:
          velVector = velVector - (throttleVector * throttle * 0.04 * g_FPSFactor)
        6:
          velVector = velVector - (throttleVector * throttle * 0.0425 * g_FPSFactor)
      end case
    end if
    if the shiftDown and (g_BoostPercent > 0) and (ferrisState < 2) then
      velVector = velVector - (throttleVector * throttle * 0.025 * g_FPSFactor)
      g_BoostPercent = g_BoostPercent - (0.005 * g_FPSFactor)
      if g_BoostPercent < 0 then
        g_BoostPercent = 0
      end if
    end if
  end if
  tempFactor = -mPlayerCar.transform.zAxis.dot(velVector)
  steerVel = steer * 0.001 * tempFactor
  if steerVel > 0.5 then
    steerVel = 0.5
  end if
  if steerVel < -0.5 then
    steerVel = -0.5
  end if
  if (imbedList[1] > 0) or (imbedList[2] > 0) then
    rotVector.y = rotVector.y - (steerVel * g_FPSFactor)
  end if
end

on doDamageMaxOutReset
  sprite(1).doPanel(32, rgb(255, 255, 255), 400)
  sprite(1).fadeToBlackBlend = 100
  g_DamagePercent = 0.0
  g_BoostPercent = 1.0
  ferrisState = 0
  racePodPointer = 1
  pointPodPointer = 1
  camState = 1
  cam = sprite(1).camera
  cam.fieldOfView = 55
  camRange = 200
  camHeight = 50
  gPlayerCar.worldPosition = vector(33650.13999999999941792, -5.55999999999999961, 33374.08000000000174623)
  mPlayerCar.transform.rotation = vector(0, 0, 0)
  camAim = gPlayerCar.worldPosition + vector(0, 50, -100)
  cam.worldPosition = gPlayerCar.worldPosition + vector(0, 200, -300)
  velVector = vector(0, 0, 0)
  rotVector = vector(0, 0, 0)
  if g_Muted = 0 then
    puppetSound(1, member(24, "Sfx"))
    soundMemberList[1] = member(24, "Sfx")
  end if
  mBoot.transform.rotation = vector(0, 0, 0)
  mBonnet.transform.rotation = vector(0, 0, 0)
  mDoorL.transform.rotation = vector(0, 0, 0)
  mDoorR.transform.rotation = vector(0, 0, 0)
  partRotList = [0, 0, 0, 0, 0, 0, 0]
  partRotVelList = [0, 0, 0, 0, 0, 0, 0]
end

on doGravity
  if midAir = 1 then
    velVector.y = velVector.y - (1.0 * g_FPSFactor)
  else
    velVector.y = velVector.y - (1.0 * g_FPSFactor)
  end if
end

on doFriction
  if midAir = 0 then
    repeat with i = g_FPSFactorDec down to 0
      if i = 0 then
        velVector.y = velVector.y - (velVector.y * 0.05 * g_FPSFactorFrac)
        velVector = velVector - (velVector * 0.005 * g_FPSFactorFrac)
        rotVector = rotVector - (rotVector * 0.16 * g_FPSFactorFrac)
        next repeat
      end if
      velVector.y = velVector.y - (velVector.y * 0.05)
      velVector = velVector - (velVector * 0.005)
      rotVector = rotVector - (rotVector * 0.16)
    end repeat
  else
    g_MidAirTime = g_MidAirTime + integer(2 * g_FPSFactor)
    repeat with i = g_FPSFactorDec down to 0
      if i = 0 then
        next repeat
      end if
    end repeat
  end if
end

on doRadialPush paramTile, paramObject, paramRadius, paramPushOrigin, paramZeroY, paramPlaySfx
  diffVector = mVisTileList[paramTile].worldPosition + paramPushOrigin - (paramObject.worldPosition + (velVector * g_FPSFactor))
  if paramZeroY = 1 then
    diffVector.y = 0
  end if
  if diffVector.magnitude < paramRadius then
    pushVector = diffVector.getNormalized() * (paramRadius - diffVector.magnitude)
    velVector = velVector - (pushVector / g_FPSFactor)
    rotVector.z = rotVector.z - (pushVector.x * 0.40000000000000002 / g_FPSFactor)
    rotVector.x = rotVector.x + (pushVector.z * 0.40000000000000002 / g_FPSFactor)
    pushMag = pushVector.magnitude / g_FPSFactor
    if paramPlaySfx = 1 then
      if pushMag > 20 then
        g_DamagePercent = g_DamagePercent + 0.02
        if g_Muted = 0 then
          puppetSound(1, member(19, "Sfx"))
          soundMemberList[1] = member(19, "Sfx")
        end if
      else
        if pushMag > 15 then
          g_DamagePercent = g_DamagePercent + 0.015
          if g_Muted = 0 then
            puppetSound(1, member(18, "Sfx"))
            soundMemberList[1] = member(18, "Sfx")
          end if
        else
          if pushMag > 10 then
            g_DamagePercent = g_DamagePercent + 0.01
            if g_Muted = 0 then
              puppetSound(1, member(17, "Sfx"))
              soundMemberList[1] = member(17, "Sfx")
            end if
          else
            if pushMag > 5 then
              g_DamagePercent = g_DamagePercent + 0.005
              if g_Muted = 0 then
                puppetSound(1, member(16, "Sfx"))
                soundMemberList[1] = member(16, "Sfx")
              end if
            end if
          end if
        end if
      end if
    end if
  end if
end

on doCollisions
  tempRadius = 140
  tempiCount = mTrafficList.count
  repeat with i = 1 to tempiCount
    if trafficStateList[i] = 1 then
      diffVector = mTrafficList[i].worldPosition - (gPlayerCar.worldPosition + (velVector * g_FPSFactor))
      if diffVector.magnitude < tempRadius then
        pushVector = diffVector.getNormalized() * (tempRadius - diffVector.magnitude)
        velVector = velVector - (pushVector / g_FPSFactor)
        rotVector.z = rotVector.z - (pushVector.x * 0.10000000000000001 / g_FPSFactor)
        rotVector.x = rotVector.x + (pushVector.z * 0.10000000000000001 / g_FPSFactor)
        pushMag = pushVector.magnitude / g_FPSFactor
        if pushMag > 5 then
          g_DamagePercent = g_DamagePercent + 0.002
          if g_Muted = 0 then
            puppetSound(1, member(16, "Sfx"))
          end if
        end if
        if pushMag > 15 then
          g_DamagePercent = g_DamagePercent + 0.005
          if g_Muted = 0 then
            puppetSound(1, member(18, "Sfx"))
          end if
        end if
      end if
    end if
  end repeat
  visTileCollisionList = [visOnTile]
  if (onTileX < 2500) and (onTilez < 2500) then
    visTileCollisionList.add(visOnTile - 1)
    visTileCollisionList.add(visOnTile - visTileCols)
    visTileCollisionList.add(visOnTile - visTileCols - 1)
  end if
  if (onTileX >= 2500) and (onTilez < 2500) then
    visTileCollisionList.add(visOnTile + 1)
    visTileCollisionList.add(visOnTile - visTileCols + 1)
    visTileCollisionList.add(visOnTile - visTileCols)
  end if
  if (onTileX < 2500) and (onTilez >= 2500) then
    visTileCollisionList.add(visOnTile - 1)
    visTileCollisionList.add(visOnTile + visTileCols - 1)
    visTileCollisionList.add(visOnTile + visTileCols)
  end if
  if (onTileX >= 2500) and (onTilez >= 2500) then
    visTileCollisionList.add(visOnTile + 1)
    visTileCollisionList.add(visOnTile + visTileCols + 1)
    visTileCollisionList.add(visOnTile + visTileCols)
  end if
  repeat with i = 1 to 4
    tempTile = visTileCollisionList[i]
    tempTileID = visTileIDList[visTileCollisionList[i]]
    tempTileRot = visTileRotList[tempTile]
    case tempTileID of
      1:
      2:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          if pushOrigin.x < 700 then
            pushOrigin.x = 700
          end if
          if pushOrigin.x > 2500 then
            pushOrigin.x = 2500
          end if
        else
          if pushOrigin.x > -700 then
            pushOrigin.x = -700
          end if
          if pushOrigin.x < -2500 then
            pushOrigin.x = -2500
          end if
        end if
        if pushOrigin.z > 0 then
          if pushOrigin.z < 700 then
            pushOrigin.z = 700
          end if
          if pushOrigin.z > 2500 then
            pushOrigin.z = 2500
          end if
        else
          if pushOrigin.z > -700 then
            pushOrigin.z = -700
          end if
          if pushOrigin.z < -2500 then
            pushOrigin.z = -2500
          end if
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          pushOrigin.x = 500
          pushOrigin.z = 500
        else
          pushOrigin.x = -500
          pushOrigin.z = -500
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      3:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z > 350 then
          pushOrigin.z = 350
        end if
        if pushOrigin.z < -350 then
          pushOrigin.z = -350
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if (abs(pushOrigin.x) < 680) and (pushOrigin.y < 100) then
          if pushOrigin.x < 0 then
            pushOrigin.x = -680
          else
            pushOrigin.x = 680
          end if
        else
          if pushOrigin.z < 0 then
            pushOrigin.z = -350
          else
            pushOrigin.z = 350
          end if
        end if
        tempRadius = 200
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      4:
        pushOrigin = vector(0, 0, 0)
        tempRadius = 700
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      5:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z >= 500 then
          if pushOrigin.x < -2225 then
            pushOrigin.x = -2225
          end if
          if pushOrigin.x > 2225 then
            pushOrigin.x = 2225
          end if
          if pushOrigin.z > 0 then
            pushOrigin.z = 725
          end if
          if pushOrigin.z < 0 then
            pushOrigin.z = -2225
          end if
          if pushOrigin.y < 100 then
            if pushOrigin.x > -550 then
              if pushOrigin.x < -350 then
                pushOrigin.x = -350
              end if
            else
              if pushOrigin.x > -1000 then
                pushOrigin.x = -770
              else
                if pushOrigin.x > -1150 then
                  if pushOrigin.x > -1150 then
                    pushOrigin.x = -1150
                  end if
                end if
              end if
            end if
          end if
        else
          if pushOrigin.z > 0 then
            pushOrigin.z = 150
            if pushOrigin.x < -2400 then
              pushOrigin.x = -2400
            end if
            if pushOrigin.x > -2225 then
              pushOrigin.x = -2225
            end if
          else
            if pushOrigin.z > -500 then
              pushOrigin.z = -300
              if pushOrigin.x < -2400 then
                pushOrigin.x = -2400
              end if
              if pushOrigin.x > -2225 then
                pushOrigin.x = -2225
              end if
            else
              if pushOrigin.z > -1000 then
                pushOrigin.z = -750
                if pushOrigin.x > 1850 then
                  if pushOrigin.y > 80 then
                    pushOrigin.x = 1850
                  else
                    if pushOrigin.x > 2225 then
                      pushOrigin.x = 2225
                    end if
                  end if
                else
                  if pushOrigin.x > -750 then
                    if (pushOrigin.x < -400) and (pushOrigin.y < 800) then
                      pushOrigin.x = -400
                    end if
                  else
                    if pushOrigin.x > -2225 then
                      if (pushOrigin.x > -1150) and (pushOrigin.y < 800) then
                        pushOrigin.x = -1150
                      end if
                      if pushOrigin.x < -1850 then
                        pushOrigin.x = -1850
                      end if
                    else
                      pushOrigin.x = -1850
                    end if
                  end if
                end if
              else
                if pushOrigin.z > -2375 then
                  pushOrigin.z = -2225
                  if pushOrigin.x < -2225 then
                    pushOrigin.x = -2225
                  end if
                  if pushOrigin.x > 2225 then
                    pushOrigin.x = 2225
                  end if
                else
                  pushOrigin.z = -2500
                  if pushOrigin.x < -2500 then
                    pushOrigin.x = -2500
                  end if
                  if pushOrigin.x > 2500 then
                    pushOrigin.x = 2500
                  end if
                end if
              end if
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 2050 then
          pushOrigin.x = 2225
          if pushOrigin.z > 700 then
            pushOrigin.z = 700
          end if
          if pushOrigin.z < -2225 then
            pushOrigin.z = -2225
          end if
        else
          if pushOrigin.x > 1500 then
            pushOrigin.x = 1850
            if pushOrigin.z < -1050 then
              pushOrigin.z = -1050
            end if
            if pushOrigin.z > -450 then
              pushOrigin.z = -450
            end if
          else
            if pushOrigin.x > 500 then
              pushOrigin.x = 1000
              if pushOrigin.y < 700 then
                if pushOrigin.z > -750 then
                  pushOrigin.z = 350
                else
                  pushOrigin.z = -1870
                end if
              else
                pushOrigin.z = -750
              end if
            else
              if pushOrigin.x > -200 then
                pushOrigin.x = 0
                if pushOrigin.y < 700 then
                  if pushOrigin.z > -750 then
                    pushOrigin.z = 350
                  else
                    pushOrigin.z = -1870
                  end if
                else
                  pushOrigin.z = -750
                end if
              else
                if pushOrigin.x > -600 then
                  pushOrigin.x = -400
                  if pushOrigin.z < -1050 then
                    pushOrigin.z = -1050
                  end if
                  if pushOrigin.z > -450 then
                    pushOrigin.z = -450
                  end if
                else
                  if pushOrigin.x > -950 then
                    pushOrigin.x = -770
                    if pushOrigin.y < 800 then
                      if pushOrigin.z < -1050 then
                        pushOrigin.z = -1050
                      end if
                      if pushOrigin.y > 100 then
                        if pushOrigin.z > -450 then
                          pushOrigin.z = -450
                        end if
                      else
                        if pushOrigin.z > 700 then
                          pushOrigin.z = 700
                        end if
                      end if
                    else
                      pushOrigin.z = -750
                    end if
                  else
                    if pushOrigin.x > -1500 then
                      pushOrigin.x = -1150
                      if (pushOrigin.z < -1500) and (pushOrigin.y < 700) then
                        pushOrigin.z = -1870
                      else
                        if pushOrigin.z < -1050 then
                          pushOrigin.z = -1050
                        end if
                        if pushOrigin.y > 100 then
                          if (pushOrigin.z > 0) and (pushOrigin.y < 800) then
                            pushOrigin.z = 320
                          else
                            if pushOrigin.z > -450 then
                              pushOrigin.z = -450
                            end if
                          end if
                        else
                          if pushOrigin.z > 700 then
                            pushOrigin.z = 700
                          end if
                        end if
                      end if
                    else
                      if pushOrigin.x > -2000 then
                        pushOrigin.x = -1850
                        if pushOrigin.z < -1050 then
                          pushOrigin.z = -1050
                        end if
                        if pushOrigin.z > -450 then
                          pushOrigin.z = -450
                        end if
                      else
                        if pushOrigin.x > -2350 then
                          pushOrigin.x = -2225
                          if pushOrigin.z > 700 then
                            pushOrigin.z = 700
                          end if
                          if pushOrigin.z < -2225 then
                            pushOrigin.z = -2225
                          end if
                        else
                          pushOrigin.x = -2420
                          if pushOrigin.z > 140 then
                            pushOrigin.z = 140
                          end if
                          if pushOrigin.z < -300 then
                            pushOrigin.z = -300
                          end if
                        end if
                      end if
                    end if
                  end if
                end if
              end if
            end if
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      6:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            pushOrigin.x = 2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            else
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
            end if
          1:
            pushOrigin.x = 2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            else
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
            end if
          2:
            pushOrigin.x = -2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            else
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
            end if
          3:
            pushOrigin.x = -2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            else
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            pushOrigin.z = -2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            else
              if pushOrigin.x < -2500 then
                pushOrigin.x = -2500
              end if
            end if
          1:
            pushOrigin.z = 2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            else
              if pushOrigin.x < -2500 then
                pushOrigin.x = -2500
              end if
            end if
          2:
            pushOrigin.z = 2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            else
              if pushOrigin.x < -2500 then
                pushOrigin.x = -2500
              end if
            end if
          3:
            pushOrigin.z = -2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            else
              if pushOrigin.x < -2500 then
                pushOrigin.x = -2500
              end if
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      7:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if (pushOrigin.x < 0) and (pushOrigin.x > -650) then
              pushOrigin.x = -650
            end if
            if (pushOrigin.x >= 0) and (pushOrigin.x < 650) then
              pushOrigin.x = 650
            end if
            if pushOrigin.z > 2450 then
              pushOrigin.z = 2450
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          2:
            if pushOrigin.x < -2450 then
              pushOrigin.x = -2450
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if (pushOrigin.x < 0) and (pushOrigin.x > -650) then
              pushOrigin.x = -650
            end if
            if (pushOrigin.x >= 0) and (pushOrigin.x < 650) then
              pushOrigin.x = 650
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      8:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        pushOrigin.y = 500
        if pushOrigin.x > 0 then
          pushOrigin.x = 650
        else
          if pushOrigin.x > -2000 then
            pushOrigin.x = -650
          else
            pushOrigin.x = -2500
          end if
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      9:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        pushOrigin.z = -2500
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        case onTile of
          4:
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.x < -780 then
              pushOrigin.x = -780
            end if
            if pushOrigin.x > 580 then
              pushOrigin.x = 580
            end if
            if pushOrigin.z < -2320 then
              pushOrigin.z = -2320
            end if
            if pushOrigin.z > -700 then
              pushOrigin.z = -700
            end if
            tempRadius = 140
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.x < 0 then
              if pushOrigin.x < -980 then
                pushOrigin.x = -980
              end if
              if pushOrigin.z < -1900 then
                pushOrigin.z = -1900
              end if
              if pushOrigin.z > -1180 then
                pushOrigin.z = -1180
              end if
            else
              if pushOrigin.x > 780 then
                pushOrigin.x = 780
              end if
              if pushOrigin.z < -1560 then
                pushOrigin.z = -1560
              end if
              if pushOrigin.z > -1180 then
                pushOrigin.z = -1180
              end if
            end if
            tempRadius = 140
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
          5:
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.x < -600 then
              pushOrigin.x = -1270
              pushOrigin.z = -1700
              tempRadius = 560
            else
              pushOrigin.z = -1700
              if pushOrigin.x < 50 then
                pushOrigin.x = 50
              end if
              if pushOrigin.x > 1250 then
                pushOrigin.x = 1250
              end if
              tempRadius = 440
            end if
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
          6:
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            pushOrigin.x = 0
            pushOrigin.z = -1700
            tempRadius = 760
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        end case
      10:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          pushOrigin.x = 720
        else
          pushOrigin.x = -720
        end if
        if pushOrigin.z > 0 then
          pushOrigin.z = 720
        else
          pushOrigin.z = -720
        end if
        tempRadius = 320
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      11:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          if pushOrigin.x < 700 then
            pushOrigin.x = 700
          end if
          if pushOrigin.x > 2500 then
            pushOrigin.x = 2500
          end if
          if pushOrigin.z > 0 then
            if pushOrigin.z < 700 then
              pushOrigin.z = 700
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
          else
            if pushOrigin.z > -700 then
              pushOrigin.z = -700
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          end if
        else
          if pushOrigin.z > 0 then
            if pushOrigin.x > -700 then
              pushOrigin.x = -700
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.z < 700 then
              pushOrigin.z = 700
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
          else
            if pushOrigin.x > -1000 then
              pushOrigin.x = -1000
            end if
            if pushOrigin.x < -2000 then
              pushOrigin.x = -2000
            end if
            if pushOrigin.z > -1000 then
              pushOrigin.z = -1000
            end if
            if pushOrigin.z < -2000 then
              pushOrigin.z = -2000
            end if
          end if
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          pushOrigin.x = 500
          pushOrigin.z = 500
        else
          pushOrigin.x = -500
          pushOrigin.z = -500
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 1, 1)
      12:
        case tempTileRot of
          0:
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.y > 150 then
              pushOrigin.y = 150
            end if
            if pushOrigin.z > 0 then
              tempRadius = 120
              pushOrigin.z = 310
              if pushOrigin.x > 320 then
                pushOrigin.x = 320
              end if
              if pushOrigin.x < -320 then
                pushOrigin.x = -320
              end if
            else
              if pushOrigin.z > -700 then
                tempRadius = 140
                pushOrigin.z = -360
                if pushOrigin.x > 0 then
                  pushOrigin.x = 1420
                else
                  pushOrigin.x = -1420
                end if
              else
                tempRadius = 460
                pushOrigin.x = 0
                pushOrigin.y = 0
                pushOrigin.z = -1240
              end if
            end if
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if abs(pushOrigin.x) < 540 then
              tempRadius = 120
              if pushOrigin.z > 2500 then
                pushOrigin.z = 2500
              end if
              if pushOrigin.z < -1250 then
                pushOrigin.z = -1250
              end if
              if pushOrigin.x < 0 then
                pushOrigin.x = -320
              else
                pushOrigin.x = 320
              end if
              tempZPercent = (pushOrigin.z - 2500) / 7500.0
              maxY = (1 - cos(360 * tempZPercent / radians)) * 250
              if (pushOrigin.y > maxY) or (pushOrigin.z < 300) then
                pushOrigin.y = maxY
              end if
            else
              tempRadius = 140
              if pushOrigin.z > -1250 then
                pushOrigin.z = -1250
              end if
              if pushOrigin.x < 0 then
                pushOrigin.y = 500
                pushOrigin.x = -680
              else
                pushOrigin.y = 500
                pushOrigin.x = 680
              end if
            end if
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.x > 0 then
              spiralOrigin = vector(1420, 0, -1240)
            else
              spiralOrigin = vector(-1420, 0, -1240)
            end if
            diffVector = spiralOrigin - pushOrigin
            diffVector.y = 0
            diffVectorNorm = diffVector.duplicate()
            diffVectorNorm.normalize()
            tempMag = diffVector.magnitude
            if tempMag < 860 then
              tempRad = 700
            else
              tempRad = 1040
            end if
            tempOffset = spiralOrigin - (diffVectorNorm * tempRad)
            if (pushOrigin.y < 200) and (pushOrigin.z > -1240) then
              if pushOrigin.x < 0 then
                tempOffset = spiralOrigin + vector(tempRad, 0, 0)
              else
                tempOffset = spiralOrigin - vector(tempRad, 0, 0)
              end if
            end if
            if (pushOrigin.y > 300) and (pushOrigin.z < -1240) then
              if pushOrigin.x < 0 then
                tempOffset = spiralOrigin + vector(tempRad, 0, 0)
              else
                tempOffset = spiralOrigin - vector(tempRad, 0, 0)
              end if
            end if
            pushOrigin.x = tempOffset.x
            pushOrigin.z = tempOffset.z
            tempRadius = 120
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
          2:
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.y > 150 then
              pushOrigin.y = 150
            end if
            if pushOrigin.z < 0 then
              tempRadius = 120
              pushOrigin.z = -310
              if pushOrigin.x > 320 then
                pushOrigin.x = 320
              end if
              if pushOrigin.x < -320 then
                pushOrigin.x = -320
              end if
            else
              if pushOrigin.z < 700 then
                tempRadius = 140
                pushOrigin.z = 360
                if pushOrigin.x > 0 then
                  pushOrigin.x = 1420
                else
                  pushOrigin.x = -1420
                end if
              else
                tempRadius = 460
                pushOrigin.x = 0
                pushOrigin.y = 0
                pushOrigin.z = 1240
              end if
            end if
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if abs(pushOrigin.x) < 540 then
              tempRadius = 120
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
              if pushOrigin.z > 1250 then
                pushOrigin.z = 1250
              end if
              if pushOrigin.x < 0 then
                pushOrigin.x = -320
              else
                pushOrigin.x = 320
              end if
              tempZPercent = (pushOrigin.z + 2500) / 7500.0
              maxY = (1 - cos(360 * tempZPercent / radians)) * 250
              if (pushOrigin.y > maxY) or (pushOrigin.z > -300) then
                pushOrigin.y = maxY
              end if
            else
              tempRadius = 140
              if pushOrigin.z < 1250 then
                pushOrigin.z = 1250
              end if
              if pushOrigin.x < 0 then
                pushOrigin.y = 500
                pushOrigin.x = -680
              else
                pushOrigin.y = 500
                pushOrigin.x = 680
              end if
            end if
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
            pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
            if pushOrigin.x > 0 then
              spiralOrigin = vector(1420, 0, 1240)
            else
              spiralOrigin = vector(-1420, 0, 1240)
            end if
            diffVector = spiralOrigin - pushOrigin
            diffVector.y = 0
            diffVectorNorm = diffVector.duplicate()
            diffVectorNorm.normalize()
            tempMag = diffVector.magnitude
            if tempMag < 860 then
              tempRad = 700
            else
              tempRad = 1040
            end if
            tempOffset = spiralOrigin - (diffVectorNorm * tempRad)
            if (pushOrigin.y < 200) and (pushOrigin.z < 1240) then
              if pushOrigin.x < 0 then
                tempOffset = spiralOrigin + vector(tempRad, 0, 0)
              else
                tempOffset = spiralOrigin - vector(tempRad, 0, 0)
              end if
            end if
            if (pushOrigin.y > 300) and (pushOrigin.z > 1240) then
              if pushOrigin.x < 0 then
                tempOffset = spiralOrigin + vector(tempRad, 0, 0)
              else
                tempOffset = spiralOrigin - vector(tempRad, 0, 0)
              end if
            end if
            pushOrigin.x = tempOffset.x
            pushOrigin.z = tempOffset.z
            tempRadius = 120
            doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        end case
      13, 14, 15:
        gPlaneCollider.worldPosition = gPlayerCar.worldPosition + (velVector * g_FPSFactor)
        gPlaneCollider.transform.position.y = 0
        if gPlaneCollider.transform.position.z > 800 then
          gPlaneCollider.transform.position.x = 0
          gPlaneCollider.transform.position.z = 1220
        else
          if gPlaneCollider.transform.position.x < -250 then
            gPlaneCollider.transform.position.x = -375
            gPlaneCollider.transform.position.z = 200
          else
            if gPlaneCollider.transform.position.x < 0 then
              gPlaneCollider.transform.position.x = -140
              gPlaneCollider.transform.position.z = 0
            else
              if gPlaneCollider.transform.position.x < 250 then
                gPlaneCollider.transform.position.x = 140
                gPlaneCollider.transform.position.z = 0
              else
                gPlaneCollider.transform.position.x = 375
                gPlaneCollider.transform.position.z = 200
              end if
            end if
          end if
        end if
        pushOrigin = gPlaneCollider.worldPosition - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      16:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        if pushOrigin.x < -2250 then
          pushOrigin.x = -2500
          if pushOrigin.z > 2500 then
            pushOrigin.z = 2500
          end if
          if pushOrigin.z < -2500 then
            pushOrigin.z = -2500
          end if
        else
          if pushOrigin.x < 0 then
            if pushOrigin.x < -2000 then
              pushOrigin.x = -2000
            end if
            if pushOrigin.x > -700 then
              pushOrigin.x = -700
            end if
            if pushOrigin.z > 0 then
              pushOrigin.z = 0
            end if
            if pushOrigin.z < -1600 then
              pushOrigin.z = -1600
            end if
          else
            tempRadius = 140
            pushOrigin.y = 0
            if pushOrigin.x < 1180 then
              pushOrigin.x = 1180
            end if
            if pushOrigin.x > 1360 then
              pushOrigin.x = 1360
            end if
            if pushOrigin.z < 880 then
              pushOrigin.z = 880
            end if
            if pushOrigin.z > 1120 then
              pushOrigin.z = 1120
            end if
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          tempRadius = 300
          pushOrigin = vector(1000, 0, 1000)
        else
          tempRadius = 300
          pushOrigin = vector(-600, 0, 0)
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = vector(1000, 200, 1000) - (mRadar.transform.xAxis * 250)
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      17:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x < -360 then
          pushOrigin.x = -360
        end if
        if pushOrigin.x > 1100 then
          pushOrigin.x = 1100
        end if
        if pushOrigin.z > 2000 then
          pushOrigin.z = 2000
        end if
        if pushOrigin.z < -2000 then
          pushOrigin.z = -2000
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x < -500 then
          pushOrigin.x = -1300
          if pushOrigin.z > -1220 then
            pushOrigin.z = -220
          else
            pushOrigin.z = -2220
          end if
        else
          if pushOrigin.x < 800 then
            pushOrigin.x = 360
            pushOrigin.z = -2480
          else
            if pushOrigin.x < 1600 then
              pushOrigin.x = 1320
              pushOrigin.z = -2480
            else
              pushOrigin.x = 1800
              if pushOrigin.z < -1520 then
                pushOrigin.z = -2000
              else
                if pushOrigin.z < -520 then
                  pushOrigin.z = -1000
                else
                  if pushOrigin.z < 500 then
                    pushOrigin.z = 0
                  else
                    if pushOrigin.z < 1500 then
                      pushOrigin.z = 1000
                    else
                      pushOrigin.z = 2000
                    end if
                  end if
                end if
              end if
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      18:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        if pushOrigin.x > -2300 then
          if pushOrigin.z > 0 then
            if pushOrigin.z > 2000 then
              pushOrigin.z = 2000
            end if
            if pushOrigin.x > 1500 then
              if pushOrigin.y > 650 then
                pushOrigin.y = 650
              end if
              if pushOrigin.z < 1040 then
                pushOrigin.z = 1040
              end if
              if pushOrigin.z > 1600 then
                pushOrigin.z = 1600
              end if
            else
              pushOrigin.y = 0
              if (pushOrigin.z < 1600) and (pushOrigin.x > -1500) then
                pushOrigin.z = 1600
              end if
            end if
          else
            if pushOrigin.z < -2000 then
              pushOrigin.z = -2000
            end if
            if pushOrigin.x > 1500 then
              if pushOrigin.y > 650 then
                pushOrigin.y = 650
              end if
              if pushOrigin.z > -1040 then
                pushOrigin.z = -1040
              end if
              if pushOrigin.z < -1600 then
                pushOrigin.z = -1600
              end if
            else
              pushOrigin.y = 0
              if (pushOrigin.z > -1600) and (pushOrigin.x > -1500) then
                pushOrigin.z = -1600
              end if
            end if
          end if
        end if
        if pushOrigin.x < -2300 then
          pushOrigin.x = -2500
          if pushOrigin.z > 2500 then
            pushOrigin.z = 2500
          end if
          if pushOrigin.z < -2500 then
            pushOrigin.z = -2500
          end if
        else
          if pushOrigin.x < -2000 then
            pushOrigin.x = -2100
          else
            if pushOrigin.x < -1500 then
              pushOrigin.x = -1900
            else
              if pushOrigin.x < -1000 then
                pushOrigin.x = -1100
              else
                if pushOrigin.x < -500 then
                  pushOrigin.x = -900
                else
                  if pushOrigin.x < 0 then
                    pushOrigin.x = -100
                  else
                    if pushOrigin.x < 500 then
                      pushOrigin.x = 100
                    else
                      if pushOrigin.x < 1000 then
                        pushOrigin.x = 900
                      else
                        if pushOrigin.x < 1500 then
                          pushOrigin.x = 1100
                        else
                          pushOrigin.x = 2150
                        end if
                      end if
                    end if
                  end if
                end if
              end if
            end if
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        if pushOrigin.y > 300 then
          pushOrigin.y = 300
        end if
        if pushOrigin.z > 0 then
          pushOrigin.z = 1600
        else
          pushOrigin.z = -1600
        end if
        if pushOrigin.x < -2090 then
          pushOrigin.x = -2090
        else
          if pushOrigin.x > 2350 then
            pushOrigin.x = 2350
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 250
        if pushOrigin.z > 0 then
          pushOrigin.x = 2250
          pushOrigin.z = 1730
        else
          pushOrigin.x = 2250
          pushOrigin.z = -1730
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      19:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            pushOrigin.x = -2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          1:
            pushOrigin.z = -2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
          2:
            pushOrigin.x = 2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          3:
            pushOrigin.z = 2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      20:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 0 then
          pushOrigin.x = 190
          pushOrigin.z = -200
        else
          pushOrigin.x = -190
          pushOrigin.z = -200
        end if
        tempRadius = 190
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 1720 then
          pushOrigin.x = 2010
          if pushOrigin.z > 2010 then
            pushOrigin.z = 2010
          end if
          if pushOrigin.z < -2010 then
            pushOrigin.z = -2010
          end if
        else
          if pushOrigin.x > 1000 then
            pushOrigin.x = 1500
            if pushOrigin.z < 1000 then
              pushOrigin.z = 1000
            end if
            if pushOrigin.z > 1500 then
              pushOrigin.z = 1500
            end if
          else
            if pushOrigin.x > 150 then
              pushOrigin.x = 510
              if pushOrigin.z < -310 then
                pushOrigin.z = -310
              end if
              if pushOrigin.z > 370 then
                pushOrigin.z = 370
              end if
            else
              if pushOrigin.x > -300 then
                pushOrigin.x = 0
                if pushOrigin.z < 1000 then
                  pushOrigin.z = 1000
                end if
                if pushOrigin.z > 1500 then
                  pushOrigin.z = 1500
                end if
              else
                if pushOrigin.x > -1200 then
                  pushOrigin.x = -510
                  if pushOrigin.z < -310 then
                    pushOrigin.z = -310
                  end if
                  if pushOrigin.z > 370 then
                    pushOrigin.z = 370
                  end if
                else
                  pushOrigin.x = -2010
                  if pushOrigin.z > 2010 then
                    pushOrigin.z = 2010
                  end if
                  if pushOrigin.z < -2010 then
                    pushOrigin.z = -2010
                  end if
                end if
              end if
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        pushOrigin.y = 0
        if pushOrigin.z < -1000 then
          pushOrigin.z = -2010
          if pushOrigin.x < -2010 then
            pushOrigin.x = -2010
          end if
          if pushOrigin.x > 2010 then
            pushOrigin.x = 2010
          end if
          if (pushOrigin.x >= 0) and (pushOrigin.x < 300) then
            pushOrigin.x = 300
          end if
          if (pushOrigin.x < 0) and (pushOrigin.x > -300) then
            pushOrigin.x = -300
          end if
        else
          if pushOrigin.z < 0 then
            pushOrigin.z = -320
            if pushOrigin.x > 510 then
              pushOrigin.x = 510
            end if
            if pushOrigin.x < -510 then
              pushOrigin.x = -510
            end if
            if (pushOrigin.x >= 0) and (pushOrigin.x < 190) then
              pushOrigin.x = 190
            end if
            if (pushOrigin.x < 0) and (pushOrigin.x > -190) then
              pushOrigin.x = -190
            end if
          else
            if pushOrigin.z < 720 then
              pushOrigin.z = 370
              if pushOrigin.x > 510 then
                pushOrigin.x = 510
              end if
              if pushOrigin.x < -510 then
                pushOrigin.x = -510
              end if
            else
              if pushOrigin.z < 1240 then
                pushOrigin.z = 980
                if pushOrigin.x > 1500 then
                  pushOrigin.x = 1500
                end if
                if pushOrigin.x < 0 then
                  pushOrigin.x = 0
                end if
                if pushOrigin.x > 1150 then
                  if pushOrigin.x < 1270 then
                    pushOrigin.x = 1270
                  end if
                else
                  if pushOrigin.x > 1030 then
                    pushOrigin.x = 1030
                  end if
                end if
              else
                if pushOrigin.z < 1700 then
                  pushOrigin.z = 1500
                  if pushOrigin.x < 0 then
                    pushOrigin.x = 0
                  end if
                  if pushOrigin.x > 1500 then
                    pushOrigin.x = 1500
                  end if
                else
                  pushOrigin.z = 2010
                  if pushOrigin.x > 2010 then
                    pushOrigin.x = 2010
                  end if
                  if pushOrigin.x < -2010 then
                    pushOrigin.x = -2010
                  end if
                end if
              end if
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 100
        if pushOrigin.z < 0 then
          if pushOrigin.x > 0 then
            pushOrigin.x = 1400
            pushOrigin.z = -1400
          else
            if pushOrigin.x > -1240 then
              pushOrigin.x = -1100
              pushOrigin.z = -1500
            else
              pushOrigin.x = -1400
              pushOrigin.z = -1500
            end if
          end if
        else
          tempRadius = 100
          if pushOrigin.z < 1200 then
            if pushOrigin.x > -1155 then
              pushOrigin.x = -1045
            else
              pushOrigin.x = -1260
            end if
            if pushOrigin.z < 1000 then
              pushOrigin.z = 1000
            end if
          else
            railOrigin = vector(-1155, 0, 1310)
            tempDiff = railOrigin - pushOrigin
            tempDiffNorm = tempDiff.getNormalized()
            pushOrigin = railOrigin - (tempDiffNorm * 142)
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      21:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.z > 0 then
          pushOrigin.z = 1500
        else
          if pushOrigin.z > -850 then
            pushOrigin.z = -500
            pushOrigin.x = 0
          else
            if pushOrigin.z > -1300 then
              pushOrigin.z = -1110
              if pushOrigin.x > 690 then
                pushOrigin.x = 690
              end if
              if pushOrigin.x < 280 then
                pushOrigin.x = 280
              end if
            else
              if pushOrigin.z > -1800 then
                pushOrigin.z = -1500
                if (pushOrigin.x > 0) and (pushOrigin.x < 280) then
                  pushOrigin.x = 280
                end if
                if (pushOrigin.x < 0) and (pushOrigin.x > -700) then
                  pushOrigin.x = -700
                end if
              else
                pushOrigin.z = -2000
                if pushOrigin.x > -155 then
                  pushOrigin.x = 0
                else
                  pushOrigin.x = -300
                end if
              end if
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.x > 500 then
          pushOrigin.x = 680
          if pushOrigin.z < -1505 then
            pushOrigin.z = -1505
          end if
          if pushOrigin.z > -1110 then
            pushOrigin.z = -1110
          end if
        else
          if pushOrigin.x > 150 then
            pushOrigin.x = 290
            if pushOrigin.z < -1505 then
              pushOrigin.z = -1505
            end if
            if pushOrigin.z > -1110 then
              pushOrigin.z = -1110
            end if
          else
            pushOrigin.x = 0
            if pushOrigin.z < -2000 then
              pushOrigin.z = -2000
            end if
            if pushOrigin.z > -500 then
              pushOrigin.z = -500
            end if
          end if
        end if
        tempRadius = 120
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      22:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            if (abs(pushOrigin.x) < 2500) and (abs(pushOrigin.z) < 2500) then
              if pushOrigin.x > pushOrigin.z then
                pushOriginNorm = pushOrigin.getNormalized()
                pushOrigin = pushOriginNorm * 2160
              else
                if (pushOrigin.x + 2500 + pushOrigin.z + 2500) > 5000 then
                  pushOrigin.x = 1500
                  if pushOrigin.z < 1500 then
                    pushOrigin.z = 1500
                  end if
                else
                  pushOrigin.z = -1500
                  if pushOrigin.x > -1500 then
                    pushOrigin.x = -1500
                  end if
                end if
              end if
            end if
          1:
            if (abs(pushOrigin.x) < 2500) and (abs(pushOrigin.z) < 2500) then
              if (pushOrigin.x + 2500 + pushOrigin.z + 2500) > 5000 then
                pushOriginNorm = pushOrigin.getNormalized()
                pushOrigin = pushOriginNorm * 2160
              else
                if pushOrigin.x > pushOrigin.z then
                  pushOrigin.x = 1500
                  if pushOrigin.z > -1500 then
                    pushOrigin.z = -1500
                  end if
                else
                  pushOrigin.z = 1500
                  if pushOrigin.x > -1500 then
                    pushOrigin.x = -1500
                  end if
                end if
              end if
            end if
          2:
            if (abs(pushOrigin.x) < 2500) and (abs(pushOrigin.z) < 2500) then
              if pushOrigin.x < pushOrigin.z then
                pushOriginNorm = pushOrigin.getNormalized()
                pushOrigin = pushOriginNorm * 2160
              else
                if (pushOrigin.x + 2500 + pushOrigin.z + 2500) > 5000 then
                  pushOrigin.z = 1500
                  if pushOrigin.x < 1500 then
                    pushOrigin.x = 1500
                  end if
                else
                  pushOrigin.x = -1500
                  if pushOrigin.z > -1500 then
                    pushOrigin.z = -1500
                  end if
                end if
              end if
            end if
          3:
            if (abs(pushOrigin.x) < 2500) and (abs(pushOrigin.z) < 2500) then
              if (pushOrigin.x + 2500 + pushOrigin.z + 2500) < 5000 then
                pushOriginNorm = pushOrigin.getNormalized()
                pushOrigin = pushOriginNorm * 2160
              else
                if pushOrigin.x > pushOrigin.z then
                  pushOrigin.z = -1500
                  if pushOrigin.x < 1500 then
                    pushOrigin.x = 1500
                  end if
                else
                  pushOrigin.x = -1500
                  if pushOrigin.z < 1500 then
                    pushOrigin.z = 1500
                  end if
                end if
              end if
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            pushOrigin = vector(-2500, 0, 2500)
          1:
            pushOrigin = vector(-2500, 0, -2500)
          2:
            pushOrigin = vector(2500, 0, -2500)
          3:
            pushOrigin = vector(2500, 0, 2500)
        end case
        tempRadius = 1140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      23:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0:
            cornerOrigin = vector(-2500, 0, 2500)
          1:
            cornerOrigin = vector(-2500, 0, -2500)
          2:
            cornerOrigin = vector(2500, 0, -2500)
          3:
            cornerOrigin = vector(2500, 0, 2500)
        end case
        tempDiff = pushOrigin - cornerOrigin
        tempDiffNorm = tempDiff.getNormalized()
        tempMag = tempDiff.magnitude
        if tempMag < 2500 then
          tempRad = 1000
        else
          tempRad = 4000
        end if
        pushOrigin = cornerOrigin + (tempDiffNorm * tempRad)
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      24:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0, 2:
            if pushOrigin.z > 400 then
              pushOrigin.z = 400
            end if
            if pushOrigin.z < -400 then
              pushOrigin.z = -400
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if (abs(pushOrigin.x) < 680) and (pushOrigin.y < 100) then
              if pushOrigin.x < 0 then
                pushOrigin.x = -680
              else
                pushOrigin.x = 680
              end if
            else
              if pushOrigin.z < 0 then
                pushOrigin.z = -400
              else
                pushOrigin.z = 400
              end if
            end if
          1, 3:
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      25:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0, 2:
            if pushOrigin.x > 0 then
              pushOrigin.x = 1500
            else
              pushOrigin.x = -1500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
          1, 3:
            if pushOrigin.z > 0 then
              pushOrigin.z = 1500
            else
              pushOrigin.z = -1500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      26:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0, 2:
            if pushOrigin.x > 0 then
              pushOrigin.x = 1500
            else
              pushOrigin.x = -1500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
          1, 3:
            if pushOrigin.z > 0 then
              pushOrigin.z = 1500
            else
              pushOrigin.z = -1500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          0, 2:
            if pushOrigin.x > 0 then
              pushOrigin.x = 600
            else
              pushOrigin.x = -600
            end if
            if pushOrigin.z < -1650 then
              pushOrigin.z = -1650
            end if
            if pushOrigin.z > 1650 then
              pushOrigin.z = 1650
            end if
          1, 3:
            if pushOrigin.z > 0 then
              pushOrigin.z = 600
            else
              pushOrigin.z = -600
            end if
            if pushOrigin.x < -1650 then
              pushOrigin.x = -1650
            end if
            if pushOrigin.x > 1650 then
              pushOrigin.x = 1650
            end if
        end case
        if pushOrigin.y > 100 then
          pushOrigin.y = 700
          pushOrigin.x = 0
          pushOrigin.z = 0
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      27:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        case tempTileRot of
          0, 3:
            pushOrigin.x = -2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          1, 2:
            pushOrigin.x = 2500
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
        end case
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        case tempTileRot of
          0, 1:
            pushOrigin.z = -2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
          2, 3:
            pushOrigin.z = 2500
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
        end case
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      28:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempRadius = 140
        if pushOrigin.x > 2050 then
          pushOrigin.x = 2500
          if pushOrigin.z > 2500 then
            pushOrigin.z = 2500
          end if
          if pushOrigin.z < -2500 then
            pushOrigin.z = -2500
          end if
        else
          if pushOrigin.x > 1550 then
            pushOrigin.x = 1800
            if pushOrigin.z < -1750 then
              pushOrigin.z = -2500
            else
              pushOrigin.z = 0
            end if
          else
            if pushOrigin.x > 800 then
              pushOrigin.x = 1100
              if pushOrigin.z > 2500 then
                pushOrigin.z = 2500
              end if
              if pushOrigin.z < -2500 then
                pushOrigin.z = -2500
              end if
            else
              pushOrigin.x = -250
              tempRadius = 850
              if pushOrigin.z > 0 then
                pushOrigin.z = 1250
              else
                pushOrigin.z = -1250
              end if
            end if
          end if
        end if
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      29:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempFactor = 1 - ((pushOrigin.z + 2500) / 5000.0)
        tempMaxY = -200 + (450 * tempFactor)
        if pushOrigin.y > tempMaxY then
          pushOrigin.y = tempMaxY
        end if
        if pushOrigin.x > 0 then
          pushOrigin.x = 300
        else
          pushOrigin.x = -300
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        if pushOrigin.y > 100 then
          pushOrigin.y = 100
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      30:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempZFactor = (pushOrigin.z + 2500) / 5000.0
        if tempZFactor < 0 then
          tempZFactor = 0
        end if
        if tempZFactor > 1 then
          tempZFactor = 1
        end if
        tempXOrigin = -1000 * (-1 + cos(-180 * tempZFactor / radians))
        if pushOrigin.x > tempXOrigin then
          pushOrigin.x = tempXOrigin + 300
        else
          pushOrigin.x = tempXOrigin - 300
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y < 0 then
          pushOrigin.y = 0
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      31:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        case tempTileRot of
          2:
            tempOrigin = vector(-2500, 0, -2500)
            tempDiff = tempOrigin - pushOrigin
            tempDiff.y = 0
            tempDiffNorm = tempDiff.getNormalized()
            if tempDiff.magnitude > 4500 then
              pushOrigin.x = tempOrigin.x - (tempDiffNorm.x * 4800)
              pushOrigin.z = tempOrigin.z - (tempDiffNorm.z * 4800)
            else
              pushOrigin.x = tempOrigin.x - (tempDiffNorm.x * 4200)
              pushOrigin.z = tempOrigin.z - (tempDiffNorm.z * 4200)
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
          3:
            tempOrigin = vector(2500, 0, -2500)
            tempDiff = tempOrigin - pushOrigin
            tempDiff.y = 0
            tempDiffNorm = tempDiff.getNormalized()
            if tempDiff.magnitude > 4500 then
              pushOrigin.x = tempOrigin.x - (tempDiffNorm.x * 4800)
              pushOrigin.z = tempOrigin.z - (tempDiffNorm.z * 4800)
            else
              pushOrigin.x = tempOrigin.x - (tempDiffNorm.x * 4200)
              pushOrigin.z = tempOrigin.z - (tempDiffNorm.z * 4200)
            end if
            if pushOrigin.x > 2500 then
              pushOrigin.x = 2500
            end if
            if pushOrigin.x < -2500 then
              pushOrigin.x = -2500
            end if
            if pushOrigin.z > 2500 then
              pushOrigin.z = 2500
            end if
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
        end case
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      32:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z > 2000 then
          pushOrigin.z = 2300
        else
          pushOrigin.z = 1700
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if pushOrigin.y > -140 then
          pushOrigin.y = -140
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y < 0 then
          pushOrigin.y = 0
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      33:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempZFactor = (pushOrigin.z + 2500) / 5000.0
        if tempZFactor < 0 then
          tempZFactor = 0
        end if
        if tempZFactor > 1 then
          tempZFactor = 1
        end if
        tempXOrigin = 1000 * (-1 + cos(-180 * tempZFactor / radians))
        if pushOrigin.x > tempXOrigin then
          pushOrigin.x = tempXOrigin + 300
        else
          pushOrigin.x = tempXOrigin - 300
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y < 0 then
          pushOrigin.y = 0
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        end if
        if pushOrigin.x < -2500 then
          pushOrigin.x = -2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      34:
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        tempZFactor = (pushOrigin.z + 2500) / 5000.0
        if tempZFactor < 0 then
          tempZFactor = 0
        end if
        if tempZFactor > 1 then
          tempZFactor = 1
        end if
        tempXOrigin = 1800 + (900 * (-1 + cos(-180 * tempZFactor / radians)))
        if pushOrigin.x > tempXOrigin then
          pushOrigin.x = tempXOrigin + 700
        else
          pushOrigin.x = tempXOrigin - 700
        end if
        if pushOrigin.z > 2500 then
          pushOrigin.z = 2500
        end if
        if pushOrigin.z < -2500 then
          pushOrigin.z = -2500
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z < -1500 then
          pushOrigin.x = 1800
          pushOrigin.z = -2500
        else
          pushOrigin.x = 900
          pushOrigin.z = 0
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      35:
        if gPlayerCar.worldPosition.y < -150 then
          g_DamagePercent = g_DamagePercent + 0.001
        end if
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y > -120 then
          pushOrigin.y = -120
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        else
          if pushOrigin.x < -2500 then
            pushOrigin.x = -2500
          end if
        end if
        if pushOrigin.z > 0 then
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, 1400)
            else
              tempOrigin = vector(-700, 0, 1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = 1000
          end if
        else
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, -1400)
            else
              tempOrigin = vector(-700, 0, -1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = -1000
          end if
        end if
        if pushOrigin.z > 1400 then
          pushOrigin.z = 1400
        end if
        if pushOrigin.z < -1400 then
          pushOrigin.z = -1400
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z < 0 then
          if abs(pushOrigin.x) <= 660 then
            if pushOrigin.z < -2500 then
              pushOrigin.z = -2500
            end if
            if pushOrigin.z > -1400 then
              pushOrigin.z = -1400
            end if
            if pushOrigin.y > -120 then
              pushOrigin.y = -120
            end if
          else
            pushOrigin.z = -1250
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 0 then
                  pushOrigin.x = 300
                else
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1520 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        else
          if abs(pushOrigin.x) <= 660 then
            pushOrigin.z = 1400
          else
            pushOrigin.z = 1250
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 300 then
                  pushOrigin.x = 300
                end if
                if pushOrigin.x < -300 then
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1520 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        end if
        tempRadius = 100
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      36:
        if gPlayerCar.worldPosition.y < -150 then
          g_DamagePercent = g_DamagePercent + 0.001
        end if
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y > -120 then
          pushOrigin.y = -120
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        else
          if pushOrigin.x < -2500 then
            pushOrigin.x = -2500
          end if
        end if
        if pushOrigin.z > 0 then
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, 1400)
            else
              tempOrigin = vector(-700, 0, 1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = 1000
          end if
        else
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, -1400)
            else
              tempOrigin = vector(-700, 0, -1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = -1000
          end if
        end if
        if pushOrigin.z > 1400 then
          pushOrigin.z = 1400
        end if
        if pushOrigin.z < -1400 then
          pushOrigin.z = -1400
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z < 0 then
          if abs(pushOrigin.x) <= 660 then
            pushOrigin.z = -1400
            if pushOrigin.y > -120 then
              pushOrigin.y = -120
            end if
          else
            pushOrigin.z = -1250
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 300 then
                  pushOrigin.x = 300
                end if
                if pushOrigin.x < -300 then
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1520 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        else
          if abs(pushOrigin.x) <= 660 then
            pushOrigin.z = 1400
            if pushOrigin.y > -120 then
              pushOrigin.y = -120
            end if
          else
            pushOrigin.z = 1250
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 300 then
                  pushOrigin.x = 300
                end if
                if pushOrigin.x < -300 then
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1520 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        end if
        tempRadius = 100
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y < 0 then
          pushOrigin.y = 0
        end if
        if pushOrigin.z > 1400 then
          pushOrigin.z = 1400
        else
          if pushOrigin.z < -1400 then
            pushOrigin.z = -1400
          end if
        end if
        if pushOrigin.x > 0 then
          pushOrigin.x = 320
        else
          pushOrigin.x = -320
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
      37:
        if gPlayerCar.worldPosition.y < -150 then
          g_DamagePercent = g_DamagePercent + 0.001
        end if
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.y > -120 then
          pushOrigin.y = -120
        end if
        if pushOrigin.x > 2500 then
          pushOrigin.x = 2500
        else
          if pushOrigin.x < -2500 then
            pushOrigin.x = -2500
          end if
        end if
        if pushOrigin.z > 0 then
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, 1400)
            else
              tempOrigin = vector(-700, 0, 1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = 1000
          end if
        else
          if abs(pushOrigin.x) < 700 then
            if pushOrigin.x > 0 then
              tempOrigin = vector(700, 0, -1400)
            else
              tempOrigin = vector(-700, 0, -1400)
            end if
            tempDiff = pushOrigin - tempOrigin
            tempDiff.y = 0
            tempDiff.normalize()
            pushOrigin.x = tempOrigin.x + (tempDiff.x * 400)
            pushOrigin.z = tempOrigin.z + (tempDiff.z * 400)
          else
            pushOrigin.z = -1000
          end if
        end if
        if pushOrigin.z > 1400 then
          pushOrigin.z = 1400
        end if
        if pushOrigin.z < -1400 then
          pushOrigin.z = -1400
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z > 0 then
          if abs(pushOrigin.x) <= 660 then
            pushOrigin.z = 1000
            if pushOrigin.y > -120 then
              pushOrigin.y = -120
            end if
          else
            pushOrigin.z = 1250
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 300 then
                  pushOrigin.x = 300
                end if
                if pushOrigin.x < -300 then
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1500 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        else
          if abs(pushOrigin.x) <= 300 then
            pushOrigin.z = -1400
          else
            pushOrigin.z = -1250
          end if
          if abs(pushOrigin.x) <= 660 then
            pushOrigin.z = -1000
            if pushOrigin.y > -120 then
              pushOrigin.y = -120
            end if
          else
            pushOrigin.z = -1000
          end if
          if pushOrigin.x < -1520 then
            pushOrigin.x = -2000
          else
            if pushOrigin.x < -660 then
              pushOrigin.x = -1000
            else
              if pushOrigin.x < 660 then
                if pushOrigin.x > 300 then
                  pushOrigin.x = 300
                end if
                if pushOrigin.x < -300 then
                  pushOrigin.x = -300
                end if
              else
                if pushOrigin.x < 1500 then
                  pushOrigin.x = 1000
                else
                  pushOrigin.x = 2000
                end if
              end if
            end if
          end if
        end if
        tempRadius = 100
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
        pushOrigin = gPlayerCar.worldPosition + (velVector * g_FPSFactor) - mVisTileList[tempTile].worldPosition
        if pushOrigin.z > 0 then
          if pushOrigin.y > 120 then
            pushOrigin.y = 120
          end if
        end if
        if pushOrigin.y < 0 then
          pushOrigin.y = 0
        end if
        if pushOrigin.z > 1250 then
          pushOrigin.z = 1250
        else
          if pushOrigin.z < -1250 then
            pushOrigin.z = -1250
          end if
        end if
        if pushOrigin.x > 0 then
          pushOrigin.x = 320
        else
          pushOrigin.x = -320
        end if
        tempRadius = 140
        doRadialPush(tempTile, gPlayerCar, tempRadius, pushOrigin, 0, 1)
    end case
  end repeat
  tempX = gPlayerCar.worldPosition.x + (velVector.x * g_FPSFactor)
  tempZ = gPlayerCar.worldPosition.z + (velVector.z * g_FPSFactor)
  tempMag = 0
  if tempX > 59860 then
    velVector.x = velVector.x - ((tempX - 59860) / g_FPSFactor)
    rotVector.z = rotVector.z - ((tempX - 59860) * 0.40000000000000002 / g_FPSFactor)
    tempMag = (tempX - 59860) / g_FPSFactor
  end if
  if tempZ < 140 then
    velVector.z = velVector.z + ((140 - tempZ) / g_FPSFactor)
    rotVector.x = rotVector.x - ((140 - tempZ) * 0.40000000000000002 / g_FPSFactor)
    tempMag = (140 - tempZ) / g_FPSFactor
  end if
  if tempZ > 59860 then
    velVector.z = velVector.z - ((tempZ - 59860) / g_FPSFactor)
    rotVector.x = rotVector.x - ((tempZ - 59860) * 0.40000000000000002 / g_FPSFactor)
    tempMag = (tempZ - 59860) / g_FPSFactor
  end if
  if abs(tempMag) > 20 then
    g_DamagePercent = g_DamagePercent + 0.04
    if g_Muted = 0 then
      puppetSound(1, member(19, "Sfx"))
      soundMemberList[1] = member(19, "Sfx")
    end if
  else
    if pushMag > 15 then
      g_DamagePercent = g_DamagePercent + 0.03
      if g_Muted = 0 then
        puppetSound(1, member(18, "Sfx"))
        soundMemberList[1] = member(18, "Sfx")
      end if
    else
      if pushMag > 10 then
        g_DamagePercent = g_DamagePercent + 0.02
        if g_Muted = 0 then
          puppetSound(1, member(17, "Sfx"))
          soundMemberList[1] = member(17, "Sfx")
        end if
      else
        if pushMag > 5 then
          g_DamagePercent = g_DamagePercent + 0.01
          if g_Muted = 0 then
            puppetSound(1, member(16, "Sfx"))
            soundMemberList[1] = member(16, "Sfx")
          end if
        end if
      end if
    end if
  end if
end

on doupdate
  gPlayerCar.translate((velVector * g_FPSFactor) + vector(0, hoist * g_FPSFactor, 0), #world)
  mPlayerCar.rotate(rotVector * g_FPSFactor, #parent)
  if (g_DamagePercent >= 1.0) and (prevDamagePercent < 1.0) then
    damageStartTicks = the ticks
    trafficSpawnedLimit = 5
    if stageState > 0 then
      sprite(1).doPanel(29, rgb(255, 255, 255), 200)
      stageState = 7
      raceLightPhase = 0
      g_CurrentScore = 0
      sprite(1).timeList = [0, 0, 0, 0, 0]
      g_TimerState = 0
      sprite(1).updateTimer()
      pointsFlagStartTicks = the ticks
      mPointPod.removeFromWorld()
      mNextPointPod.removeFromWorld()
      repeat with i = 1 to mIgNitroList.count
        mIgNitroList[i].visibility = #none
      end repeat
    else
      if raceState > 0 then
        sprite(1).doPanel(30, rgb(255, 255, 255), 200)
        raceState = 7
        sprite(1).timeList = [0, 0, 0, 0, 0]
        g_TimerState = 0
        sprite(1).updateTimer()
        raceLightPhase = 0
        g_CurrentScore = 0
        raceFlagStartTicks = the ticks
        mRacePod.removeFromWorld()
        mNextRacePod.removeFromWorld()
      else
        sprite(1).doPanel(31, rgb(255, 255, 255), 300)
      end if
    end if
  end if
  if (damageStartTicks > 0) and ((the ticks - damageStartTicks) > 300) then
    put "doDamageMaxOutReset()"
    doDamageMaxOutReset()
    damageStartTicks = 0
  end if
  if g_CurrentScore > prevCurrentScore then
    g_Points = g_Points + g_CurrentScore - prevCurrentScore
  end if
end

on doAnims
  if (g_DamagePercent >= 0.59999999999999998) and (ferrisState < 2) then
    nmrBurnSmoke.emitter.region = [gPlayerCar.worldPosition + velVector + (mPlayerCar.getWorldTransform().zAxis * 105)]
  else
    nmrBurnSmoke.emitter.region = [vector(0, 10000000, 0)]
  end if
  tCanalWater.textureTransform.translate(-0.01 * g_FPSFactor, -0.01 * g_FPSFactor, 0.0)
  mRadar.rotate(0, 5 * g_FPSFactor, 0)
  if g_DamagePercent > 0.80000000000000004 then
    partStateList = [1, 1, 1, 1, 1, 1, 1, 1]
  else
    if g_DamagePercent > 0.59999999999999998 then
      partStateList = [1, 1, 1, 0, 0, 0, 0, 0]
    else
      if g_DamagePercent > 0.40000000000000002 then
        partStateList = [1, 1, 0, 0, 0, 0, 0, 0]
      else
        if g_DamagePercent > 0.20000000000000001 then
          partStateList = [1, 0, 0, 0, 0, 0, 0, 0]
        else
          partStateList = [0, 0, 0, 0, 0, 0, 0, 0]
        end if
      end if
    end if
  end if
  if imbedCapList[3] > 20 then
    imbedCapList[3] = 20
  end if
  if imbedCapList[4] > 20 then
    imbedCapList[4] = 20
  end if
  mTireFL.transform.position.y = 16 + imbedList[4]
  mTireBL.transform.position.y = 16 + imbedList[2]
  mTireFR.transform.position.y = 16 + imbedList[3]
  mTireBR.transform.position.y = 16 + imbedList[1]
  if g_DamagePercent > 0.80000000000000004 then
    mTireFL.transform.rotation.z = 40
    mTireFR.transform.rotation.z = 40
    mTireBL.transform.rotation.z = 20
    mTireBR.transform.rotation.z = 20
  else
    mTireFL.transform.rotation.z = 0
    mTireFR.transform.rotation.z = 0
    mTireBL.transform.rotation.z = 0
    mTireBR.transform.rotation.z = 0
  end if
  prevRollVel = rollVel
  rollVector = velVector.duplicate() * 1.60000000000000009
  rollVector.y = 0
  rollFactor = rollVector.magnitude
  rollVector.normalize()
  rollVel = rollVector.dot(mPlayerCar.getWorldTransform().zAxis) * rollFactor * g_FPSFactor
  mTireFL.rotate(rollVel + (throttle * 2), 0, 0, #self)
  mTireFR.rotate(rollVel + (throttle * 2), 0, 0, #self)
  mTireBL.rotate(rollVel + (throttle * 2), 0, 0, #self)
  mTireBR.rotate(rollVel + (throttle * 2), 0, 0, #self)
  mTireFL.transform.rotation.y = steer
  mTireFR.transform.rotation.y = -steer
  mIgNitroBlastL.transform.rotation.z = random(360)
  mIgNitroBlastR.transform.rotation.z = random(360)
  if (g_BoostPercent > 0) and the shiftDown and (ferrisState < 2) then
    if g_Muted = 0 then
      if prevBoosting = 0 then
        puppetSound(3, member(23, "Sfx"))
        soundMemberList[3] = member(23, "Sfx")
      end if
    end if
    prevBoosting = 1
    mIgNitroBlastL.visibility = #front
    mIgNitroBlastR.visibility = #front
    if mIgNitroBlastL.shaderList[1].blend < 100 then
      mIgNitroBlastL.shaderList[1].blend = mIgNitroBlastL.shaderList[1].blend + 5
      mIgNitroBlastL.shaderList[2].blend = mIgNitroBlastL.shaderList[2].blend + 5
    end if
    mIgNitroBlastL.transform.scale.z = mIgNitroBlastL.transform.scale.z - ((mIgNitroBlastL.transform.scale.z - 1.0) * 0.20000000000000001 * g_FPSFactor)
    mIgNitroBlastR.transform.scale.z = mIgNitroBlastL.transform.scale.z - ((mIgNitroBlastL.transform.scale.z - 1.0) * 0.20000000000000001 * g_FPSFactor)
  else
    if mIgNitroBlastL.shaderList[1].blend >= 5 then
      mIgNitroBlastL.shaderList[1].blend = mIgNitroBlastL.shaderList[1].blend - 5
      mIgNitroBlastL.shaderList[2].blend = mIgNitroBlastL.shaderList[2].blend - 5
      mIgNitroBlastL.transform.scale.z = mIgNitroBlastL.transform.scale.z - ((mIgNitroBlastL.transform.scale.z - 0.20000000000000001) * 0.20000000000000001 * g_FPSFactor)
      mIgNitroBlastR.transform.scale.z = mIgNitroBlastL.transform.scale.z - ((mIgNitroBlastL.transform.scale.z - 0.90000000000000002) * 0.20000000000000001 * g_FPSFactor)
    else
      mIgNitroBlastL.visibility = #none
      mIgNitroBlastR.visibility = #none
    end if
    prevBoosting = 0
  end if
end

on doRayCasting
  hoist = 0
  midAir = 1
  repeat with i = 1 to 12
    tempVector = vector(0, 0, 0)
    tempVector.x = gCageList[i].worldPosition.x mod 5000
    tempVector.y = gCageList[i].worldPosition.y + 60
    tempVector.z = gCageList[i].worldPosition.z mod 5000
    if tempVector.x < 0.01 then
      tempVector.x = 0.01
    end if
    if tempVector.z < 0.01 then
      tempVector.z = 0.01
    end if
    if tempVector.x > 4999.98999999999978172 then
      tempVector.x = 4999.98999999999978172
    end if
    if tempVector.z > 4999.98999999999978172 then
      tempVector.z = 4999.98999999999978172
    end if
    tempModelList = [mTileResourceList[imbedIDList[i]]]
    tempOptionsList = [#modelList: tempModelList, #levelOfDetail: #detailed]
    mTileResourceList[imbedIDList[i]].addToWorld()
    mTileResourceList[imbedIDList[i]].transform.rotation = vector(0, -90 * imbedRotList[i], 0)
    tempResult = w.modelsUnderRay(tempVector, vector(0, -1, 0), tempOptionsList)
    if tempResult.count > 0 then
      tempResult = tempResult[1]
      isectNormList[i] = tempResult.isectNormal
      isectPosList[i] = tempResult.isectPosition
      imbedList[i] = isectPosList[i].y - gCageList[i].worldPosition.y + 30
      if imbedList[i] < 0 then
        imbedList[i] = 0
      else
        midAir = 0
      end if
      imbedCapList[i] = imbedList[i]
      toCenterList[i] = gCageList[i].worldPosition - gPlayerCar.worldPosition
      if imbedCapList[i] > 20 then
        if hoist < (imbedList[i] - 20) then
          hoist = imbedList[i] - 20
        end if
        imbedCapList[i] = 20
      end if
    else
    end if
    mTileResourceList[imbedIDList[i]].removeFromWorld()
  end repeat
  repeat with i = 1 to 4
    velVector.y = velVector.y + (imbedCapList[i] * 0.045 * g_FPSFactor)
    rotVector.x = rotVector.x - (toCenterList[i].z * imbedCapList[i] * 0.0008 * g_FPSFactor)
    rotVector.z = rotVector.z + (toCenterList[i].x * imbedCapList[i] * 0.0008 * g_FPSFactor)
    if imbedList[i] > 0 then
      slideVector = isectNormList[i].duplicate()
      velVector.z = velVector.z + (isectNormList[i].z * 0.10000000000000001 * g_FPSFactor)
      velVector.x = velVector.x + (isectNormList[i].x * 0.10000000000000001 * g_FPSFactor)
    end if
  end repeat
  repeat with i = 5 to 12
    rotVector.x = rotVector.x - (toCenterList[i].z * imbedCapList[i] * 0.0003 * g_FPSFactor)
    rotVector.z = rotVector.z + (toCenterList[i].x * imbedCapList[i] * 0.0003 * g_FPSFactor)
    if imbedCapList[i] > 0 then
      velVector.x = velVector.x - (rotVector.z * 0.06 * g_FPSFactor)
      velVector.z = velVector.z + (rotVector.x * 0.06 * g_FPSFactor)
      rotVector.x = rotVector.x - (velVector.z * 0.01 * g_FPSFactor)
      rotVector.z = rotVector.z - (velVector.x * 0.01 * g_FPSFactor)
    end if
  end repeat
  hoistCap = hoist
end

on doParts
  diffVector = prevVelVector - velVector
  repeat with i = 1 to 7
    case i of
      1:
        case partStateList[i] of
          0:
          1:
            partRotVelList[i] = partRotVelList[i] + (diffVector.y * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (partRotList[i] * 0.01 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (throttle * 0.01 * g_FPSFactor)
            repeat with j = g_FPSFactorDec down to 0
              if j = 0 then
                partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02 * g_FPSFactorFrac)
                next repeat
              end if
              partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02)
            end repeat
            partRotList[i] = partRotList[i] + (partRotVelList[i] * g_FPSFactor)
            if partRotList[i] > 0 then
              partRotList[i] = 0
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            if partRotList[i] < -60 then
              partRotList[i] = -60
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            mBonnet.transform.rotation = vector(partRotList[i], 0, 0)
          2:
          3:
          4:
        end case
      2:
        case partStateList[i] of
          0:
          1:
            partRotVelList[i] = partRotVelList[i] + (diffVector.y * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] + (rotVector.y * 0.10000000000000001 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] + (throttle * 0.01 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (partRotList[i] * 0.001 * g_FPSFactor)
            repeat with j = g_FPSFactorDec down to 0
              if j = 0 then
                partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02 * g_FPSFactorFrac)
                next repeat
              end if
              partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02)
            end repeat
            partRotList[i] = partRotList[i] + (partRotVelList[i] * g_FPSFactor)
            if partRotList[i] > 0 then
              partRotList[i] = 0
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            if partRotList[i] < -80 then
              partRotList[i] = -80
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            mDoorL.transform.rotation = vector(0, partRotList[i], 0)
          2:
          3:
          4:
        end case
      3:
        case partStateList[i] of
          0:
          1:
            partRotVelList[i] = partRotVelList[i] + (diffVector.y * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (rotVector.y * 0.10000000000000001 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] + (throttle * 0.01 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (partRotList[i] * 0.001 * g_FPSFactor)
            repeat with j = g_FPSFactorDec down to 0
              if j = 0 then
                partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02 * g_FPSFactorFrac)
                next repeat
              end if
              partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02)
            end repeat
            partRotList[i] = partRotList[i] + (partRotVelList[i] * g_FPSFactor)
            if partRotList[i] > 0 then
              partRotList[i] = 0
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            if partRotList[i] < -80 then
              partRotList[i] = -80
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            mDoorR.transform.rotation = vector(0, partRotList[i], 0)
          2:
          3:
          4:
        end case
      4:
        case partStateList[i] of
          0:
            if (the shiftDown and (g_BoostPercent > 0)) or (igNitroRot > 0) then
              partRotList[i] = partRotList[i] - ((partRotList[i] - 30) * 0.10000000000000001 * g_FPSFactor)
            else
              partRotList[i] = partRotList[i] - ((partRotList[i] - 0) * 0.10000000000000001 * g_FPSFactor)
            end if
            if playerCarID = 1 then
              mBoot.transform.rotation = vector(-partRotList[i] * 3, 0, 0)
            else
              mBoot.transform.rotation = vector(partRotList[i], 0, 0)
            end if
          1:
            partRotVelList[i] = partRotVelList[i] + (diffVector.y * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (partRotList[i] * 0.01 * g_FPSFactor)
            partRotVelList[i] = partRotVelList[i] - (throttle * 0.01 * g_FPSFactor)
            repeat with j = g_FPSFactorDec down to 0
              if j = 0 then
                partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02 * g_FPSFactorFrac)
                next repeat
              end if
              partRotVelList[i] = partRotVelList[i] - (partRotVelList[i] * 0.02)
            end repeat
            partRotList[i] = partRotList[i] + (partRotVelList[i] * g_FPSFactor)
            if partRotList[i] < 0 then
              partRotList[i] = 0
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            if partRotList[i] > 60 then
              partRotList[i] = 60
              partRotVelList[i] = -partRotVelList[i] * 0.59999999999999998
            end if
            if (the shiftDown and (g_BoostPercent > 0)) or (igNitroRot > 0) then
              partRotList[i] = partRotList[i] - ((partRotList[i] - 30) * 0.10000000000000001 * g_FPSFactor)
            else
              partRotList[i] = partRotList[i] - ((partRotList[i] - 0) * 0.10000000000000001 * g_FPSFactor)
            end if
            if playerCarID = 1 then
              mBoot.transform.rotation = vector(-partRotList[i] * 3, 0, 0)
            else
              mBoot.transform.rotation = vector(partRotList[i], 0, 0)
            end if
          2:
          3:
          4:
        end case
    end case
  end repeat
end

on doCam
  if throttle > 6 then
    lookingForwards = 1
  end if
  if throttle < -7 then
    lookingForwards = -1
  end if
  if throttle < 0 then
    lookingForwards = 0
  end if
  toCamVector = cam.worldPosition - gPlayerCar.worldPosition
  toCamVector.y = 0
  toCamVector.normalize()
  sideVector = -mPlayerCar.getWorldTransform().xAxis
  sideVector.y = 0
  sideVector.normalize()
  frontVector = -mPlayerCar.getWorldTransform().zAxis
  frontVector.y = 0
  frontSign = frontVector.dot(toCamVector)
  tempRot = sideVector.dot(toCamVector) * 1
  if lookingForwards > 0 then
    if frontSign < 0 then
      if tempRot > 0 then
        tempRot = 1
      else
        tempRot = -1
      end if
    end if
  end if
  if lookingForwards < 0 then
    if frontSign > 0 then
      if tempRot > 0 then
        tempRot = 1
      else
        tempRot = -1
      end if
    end if
  end if
  if lookingForwards > 0 then
    tempRot = -tempRot
  else
    tempRot = 0
  end if
  cam.rotate(vector(0, tempRot * g_FPSFactor, 0), gPlayerCar)
  case onTileID of
    1:
      camState = 1
    2:
      camState = 1
    3:
      camState = 1
    4:
      camState = 1
    5:
      camState = 1
    6:
      camState = 1
    7:
      camState = 2
    8:
      camState = 1
    9:
      camState = 1
    10:
      camState = 1
    11:
      camState = 1
    12:
      camState = 1
    13:
      camState = 1
    14:
      camState = 1
    15:
      camState = 1
    16:
      camState = 1
    17:
      camState = 1
    18:
      camState = 1
    19:
      camState = 1
    20:
      camState = 2
    21:
      camState = 1
    22:
      camState = 1
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40:
      camState = 1
  end case
  case camState of
    1:
      repeat with i = g_FPSFactorDec down to 0
        if i = 0 then
          camRange = camRange - ((camRange - 320) * 0.05 * g_FPSFactorFrac)
          camHeight = camHeight - ((camHeight - 120) * 0.05 * g_FPSFactorFrac)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.10000000000000001 * g_FPSFactorFrac)
          next repeat
        end if
        camRange = camRange - ((camRange - 320) * 0.05)
        camHeight = camHeight - ((camHeight - 120) * 0.05)
        camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.10000000000000001)
      end repeat
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
    2:
      repeat with i = g_FPSFactorDec down to 0
        if i = 0 then
          camRange = camRange - ((camRange - 480) * 0.05 * g_FPSFactorFrac)
          camHeight = camHeight - ((camHeight - 200) * 0.05 * g_FPSFactorFrac)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.10000000000000001 * g_FPSFactorFrac)
          next repeat
        end if
        camRange = camRange - ((camRange - 480) * 0.05)
        camHeight = camHeight - ((camHeight - 200) * 0.05)
        camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.10000000000000001)
      end repeat
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
    3:
      repeat with i = g_FPSFactorDec down to 0
        if i = 0 then
          camRange = camRange - ((camRange - 550) * 0.05 * g_FPSFactorFrac)
          camHeight = camHeight - ((camHeight - 300) * 0.05 * g_FPSFactorFrac)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001 * g_FPSFactorFrac)
          next repeat
        end if
        camRange = camRange - ((camRange - 550) * 0.05)
        camHeight = camHeight - ((camHeight - 300) * 0.05)
        camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001)
      end repeat
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
    4:
      if (onTilez > 2500) and (abs(onTileX - 2500) < 500) then
        repeat with i = g_FPSFactorDec down to 0
          if i = 0 then
            camRange = camRange - ((camRange - 750) * 0.025 * g_FPSFactorFrac)
            camHeight = camHeight - ((camHeight - 350) * 0.025 * g_FPSFactorFrac)
            camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 340, 0) - camAim) * 0.14999999999999999 * g_FPSFactorFrac)
            next repeat
          end if
          camRange = camRange - ((camRange - 750) * 0.025)
          camHeight = camHeight - ((camHeight - 350) * 0.025)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 340, 0) - camAim) * 0.14999999999999999)
        end repeat
        xDiff = gPlayerCar.worldPosition.x - cam.worldPosition.x
        cam.worldPosition.x = cam.worldPosition.x + (xDiff * 0.10000000000000001 * g_FPSFactor)
      else
        repeat with i = g_FPSFactorDec down to 0
          if i = 0 then
            camRange = camRange - ((camRange - 340) * 0.025 * g_FPSFactorFrac)
            camHeight = camHeight - ((camHeight - 100) * 0.025 * g_FPSFactorFrac)
            camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.06 * g_FPSFactorFrac)
            next repeat
          end if
          camRange = camRange - ((camRange - 340) * 0.025)
          camHeight = camHeight - ((camHeight - 100) * 0.025)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 90, 0) - camAim) * 0.06)
        end repeat
      end if
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
    3:
      repeat with i = g_FPSFactorDec down to 0
        if i = 0 then
          camRange = camRange - ((camRange - 550) * 0.05 * g_FPSFactorFrac)
          camHeight = camHeight - ((camHeight - 300) * 0.05 * g_FPSFactorFrac)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001 * g_FPSFactorFrac)
          next repeat
        end if
        camRange = camRange - ((camRange - 550) * 0.05)
        camHeight = camHeight - ((camHeight - 300) * 0.05)
        camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001)
      end repeat
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
    5:
      repeat with i = g_FPSFactorDec down to 0
        if i = 0 then
          camRange = camRange - ((camRange - 650) * 0.05 * g_FPSFactorFrac)
          camHeight = camHeight - ((camHeight - 360) * 0.05 * g_FPSFactorFrac)
          camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001 * g_FPSFactorFrac)
          next repeat
        end if
        camRange = camRange - ((camRange - 650) * 0.05)
        camHeight = camHeight - ((camHeight - 360) * 0.05)
        camAim = camAim + ((mPlayerCar.worldPosition + velVector + vector(0, 190, 0) - camAim) * 0.10000000000000001)
      end repeat
      camAim.x = mPlayerCar.worldPosition.x
      camAim.z = mPlayerCar.worldPosition.z
  end case
  cam.worldPosition.y = cam.worldPosition.y + ((gPlayerCar.worldPosition.y + camHeight - cam.worldPosition.y) * 0.10000000000000001 * g_FPSFactor)
  diffVector = cam.worldPosition - gPlayerCar.worldPosition
  diffVector.y = 0
  diffVectorNorm = diffVector.duplicate()
  diffVectorNorm.normalize()
  cam.worldPosition.x = mPlayerCar.worldPosition.x + (diffVectorNorm.x * camRange)
  cam.worldPosition.z = mPlayerCar.worldPosition.z + (diffVectorNorm.z * camRange)
  cam.pointAt(camAim)
  mSkyBox.worldPosition.x = gPlayerCar.worldPosition.x
  mSkyBox.worldPosition.z = gPlayerCar.worldPosition.z
end
