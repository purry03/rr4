property w, timerTextureList, frameCounter, cam, timeList, prevTimeList, countDown, existingOverlays, overlayScale, origin, prevCurrentScore, displayScore, startTicks, deductHundredths, scoreTextureList, flashTicks, oTimeUp, waitTicks
global g_TimerState, g_CurrentScore, g_Paused, g_DamagePercent, g_NextFrame, g_ScoreList, g_SubmitItList, g_MissionID, g_Muted

on beginSprite
  w = sprite(1).member
  repeat with i = 1 to 12
  end repeat
  waitTicks = the ticks
  g_TimerState = 0
  frameCounter = 0
  flashTicks = the ticks
  timeList = [0, 0, 0, 0, 0]
  prevTimeList = [1, 1, 1, 1, 1]
  timerTextureList = []
  scoreTextureList = []
  repeat with i = 0 to 9
    tempTexture = w.newTexture("TimerNumber" & i & "Texture", #fromCastMember, member(37 + i, "textures"))
    timerTextureList.add(tempTexture)
    timerTextureList[i + 1].renderFormat = #rgba8888
  end repeat
  repeat with i = 0 to 9
    tempTexture = w.newTexture("ScoreNumber" & i & "Texture", #fromCastMember, member(25 + i, "textures"))
    scoreTextureList.add(tempTexture)
    scoreTextureList[i + 1].renderFormat = #rgba8888
  end repeat
  cam = sprite(1).camera
  existingOverlays = cam.overlay.count
  overlayScale = 1.0
  origin = point(640, 28)
  tempTexture = w.newTexture("TimerBackGroundTexture", #fromCastMember, member(47, "textures"))
  tempTexture.renderFormat = #rgba4444
  cam.addOverlay(tempTexture, origin + (point(24, 0) * overlayScale), 0)
  cam.addOverlay(timerTextureList[1], origin + point(0, 0), 0)
  cam.addOverlay(timerTextureList[1], origin + (point(52, 0) * overlayScale), 0)
  cam.addOverlay(timerTextureList[1], origin + (point(78, 0) * overlayScale), 0)
  cam.addOverlay(timerTextureList[1], origin + (point(116, 2) * overlayScale), 0)
  cam.addOverlay(timerTextureList[1], origin + (point(132, 2) * overlayScale), 0)
  cam.overlay[existingOverlays + 1].scale = overlayScale
  cam.overlay[existingOverlays + 2].scale = overlayScale
  cam.overlay[existingOverlays + 3].scale = overlayScale
  cam.overlay[existingOverlays + 4].scale = overlayScale
  cam.overlay[existingOverlays + 5].scale = overlayScale * 0.5
  cam.overlay[existingOverlays + 6].scale = overlayScale * 0.5
  updateTimer()
  prevTimeList = timeList.duplicate()
  origin = point(4, 28)
  prevCurrentScore = g_CurrentScore
  overlayScale = 1.0
  cam.addOverlay(scoreTextureList[1], origin + point(0, 0), 0)
  cam.addOverlay(scoreTextureList[1], origin + (point(26, 0) * overlayScale), 0)
  cam.addOverlay(scoreTextureList[1], origin + (point(52, 0) * overlayScale), 0)
  cam.addOverlay(scoreTextureList[1], origin + (point(78, 0) * overlayScale), 0)
  cam.addOverlay(scoreTextureList[1], origin + (point(104, 0) * overlayScale), 0)
  cam.overlay[existingOverlays + 7].scale = overlayScale
  cam.overlay[existingOverlays + 8].scale = overlayScale
  cam.overlay[existingOverlays + 9].scale = overlayScale
  cam.overlay[existingOverlays + 10].scale = overlayScale
  cam.overlay[existingOverlays + 11].scale = overlayScale
  startTicks = the ticks
end

on exitFrame
  if g_Paused = 0 then
    addHundredths = integer((the ticks - startTicks) * 1.66666669999999995)
    deductHundredths = integer((the ticks - startTicks) * 1.66666669999999995)
    startTicks = the ticks
    case g_TimerState of
      (-2):
        timeList[1] = timeList[1] + addHundredths
        repeat while timeList[1] > 9
          if timeList[1] > 9 then
            timeList[1] = timeList[1] - 10
            timeList[2] = timeList[2] + 1
          end if
          if timeList[2] > 9 then
            timeList[2] = timeList[2] - 10
            timeList[3] = timeList[3] + 1
          end if
          if timeList[3] > 9 then
            timeList[3] = timeList[3] - 10
            timeList[4] = timeList[4] + 1
          end if
          if timeList[4] > 5 then
            timeList[4] = timeList[4] - 6
            timeList[5] = timeList[5] + 1
            if timeList[5] > 9 then
              timeList[5] = 9
            end if
          end if
        end repeat
        updateTimer()
        prevTimeList = timeList.duplicate()
      (-1):
        nothing()
      0:
        nothing()
      1:
        timeList[1] = timeList[1] - deductHundredths
        repeat while timeList[1] < 0
          if timeList[1] < 0 then
            timeList[1] = timeList[1] + 10
            timeList[2] = timeList[2] - 1
          end if
          if timeList[2] < 0 then
            timeList[2] = timeList[2] + 10
            timeList[3] = timeList[3] - 1
          end if
          if timeList[3] < 0 then
            timeList[3] = timeList[3] + 10
            timeList[4] = timeList[4] - 1
          end if
          if timeList[4] < 0 then
            timeList[4] = timeList[4] + 6
            if timeList[5] > 0 then
              timeList[5] = timeList[5] - 1
            end if
          end if
          if timeList[4] > 5 then
            timeList[4] = timeList[4] - 5
            timeList[5] = timeList[5] + 1
          end if
          if (timeList[3] = 0) and (timeList[4] = 0) and (timeList[5] = 0) then
            timeList = [0, 0, 0, 0, 0]
            g_TimerState = -1
          end if
        end repeat
        updateTimer()
        prevTimeList = timeList.duplicate()
      2:
        if overlayScale < 2.0 then
          overlayScale = overlayScale + 0.05
        end if
        origin = origin + ((point(250, 180) - origin) * 0.10000000000000001)
        cam.overlay[existingOverlays + 7].scale = overlayScale
        cam.overlay[existingOverlays + 8].scale = overlayScale
        cam.overlay[existingOverlays + 9].scale = overlayScale
        cam.overlay[existingOverlays + 10].scale = overlayScale
        cam.overlay[existingOverlays + 11].scale = overlayScale
        cam.overlay[existingOverlays + 7].loc = origin + (point(0, 0) * overlayScale)
        cam.overlay[existingOverlays + 8].loc = origin + (point(26, 0) * overlayScale)
        cam.overlay[existingOverlays + 9].loc = origin + (point(52, 0) * overlayScale)
        cam.overlay[existingOverlays + 10].loc = origin + (point(78, 0) * overlayScale)
        cam.overlay[existingOverlays + 11].loc = origin + (point(104, 0) * overlayScale)
        if overlayScale >= 2.0 then
          if (g_CurrentScore + 23) <= 99999 then
            g_CurrentScore = g_CurrentScore + 23
          end if
          timeList[1] = timeList[1] - 123
          repeat while timeList[1] < 0
            if timeList[1] < 0 then
              timeList[1] = timeList[1] + 10
              timeList[2] = timeList[2] - 1
            end if
            if timeList[2] < 0 then
              timeList[2] = timeList[2] + 10
              timeList[3] = timeList[3] - 1
            end if
            if timeList[3] < 0 then
              timeList[3] = timeList[3] + 10
              timeList[4] = timeList[4] - 1
            end if
            if timeList[4] < 0 then
              timeList[4] = timeList[4] + 6
              if timeList[5] > 0 then
                timeList[5] = timeList[5] - 1
              end if
            end if
            if timeList[4] > 5 then
              timeList[4] = timeList[4] - 5
              timeList[5] = timeList[5] + 1
            end if
            if g_Muted = 0 then
              if sprite(1).soundMemberList[2] <> member(29, "Sfx") then
                puppetSound(2, member(29, "Sfx"))
                sprite(1).soundMemberList[2] = member(29, "Sfx")
              end if
            end if
            if (timeList[3] = 0) and (timeList[4] = 0) and (timeList[5] = 0) then
              g_CurrentScore = g_CurrentScore + timeList[1] + (timeList[2] * 10)
              timeList = [0, 0, 0, 0, 0]
              g_TimerState = 3
              waitTicks = the ticks
            end if
          end repeat
        end if
        updateTimer()
        displayScore = g_CurrentScore
        prevTimeList = timeList.duplicate()
      3:
        if (the ticks - waitTicks) > 450 then
          g_TimerState = 4
        end if
      4:
        if overlayScale > 0.75 then
          overlayScale = overlayScale - 0.025
        end if
        origin = origin + ((point(4, 28) - origin) * 0.10000000000000001)
        cam.overlay[existingOverlays + 7].scale = overlayScale
        cam.overlay[existingOverlays + 8].scale = overlayScale
        cam.overlay[existingOverlays + 9].scale = overlayScale
        cam.overlay[existingOverlays + 10].scale = overlayScale
        cam.overlay[existingOverlays + 11].scale = overlayScale
        cam.overlay[existingOverlays + 7].loc = origin + (point(0, 0) * overlayScale)
        cam.overlay[existingOverlays + 8].loc = origin + (point(26, 0) * overlayScale)
        cam.overlay[existingOverlays + 9].loc = origin + (point(52, 0) * overlayScale)
        cam.overlay[existingOverlays + 10].loc = origin + (point(78, 0) * overlayScale)
        cam.overlay[existingOverlays + 11].loc = origin + (point(104, 0) * overlayScale)
        if abs(overlayScale - 0.75) < 0.10000000000000001 then
          g_TimerState = 0
        end if
      5:
        timeList = [0, 0, 0, 3, 2]
        prevTimeList = [1, 1, 1, 1, 1]
        g_TimerState = 0
        updateTimer()
    end case
    updateScore()
  else
    startTicks = the ticks
  end if
end

on updateTimer
  cam.overlay[existingOverlays + 6].source = timerTextureList[timeList[1] + 1]
  cam.overlay[existingOverlays + 5].source = timerTextureList[timeList[2] + 1]
  cam.overlay[existingOverlays + 4].source = timerTextureList[timeList[3] + 1]
  cam.overlay[existingOverlays + 3].source = timerTextureList[timeList[4] + 1]
  cam.overlay[existingOverlays + 2].source = timerTextureList[timeList[5] + 1]
end

on updateScore
  displayScore = displayScore + integer((g_CurrentScore - displayScore) * 0.20000000000000001)
  if (g_CurrentScore - displayScore) < 4 then
    displayScore = g_CurrentScore
  end if
  tempScore = displayScore
  tempOnes = tempScore mod 10
  tempScore = tempScore - tempOnes
  tempTens = tempScore mod 100 / 10
  tempScore = tempScore - tempTens
  tempHundreds = tempScore mod 1000 / 100
  tempScore = tempScore - tempHundreds
  tempThousands = tempScore mod 10000 / 1000
  tempScore = tempScore - tempThousands
  tempTensThousands = tempScore mod 100000 / 10000
  cam.overlay[existingOverlays + 7].source = scoreTextureList[tempTensThousands + 1]
  cam.overlay[existingOverlays + 8].source = scoreTextureList[tempThousands + 1]
  cam.overlay[existingOverlays + 9].source = scoreTextureList[tempHundreds + 1]
  cam.overlay[existingOverlays + 10].source = scoreTextureList[tempTens + 1]
  cam.overlay[existingOverlays + 11].source = scoreTextureList[tempOnes + 1]
end
