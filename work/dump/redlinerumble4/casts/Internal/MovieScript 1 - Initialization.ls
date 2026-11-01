global g_NextFrame, g_PrevFrame, g_LevelPassedList, g_LevelScoreList, g_MissionID, g_RaceID, g_FirstTime, g_NextFramePlease, g_SaveGameName, g_RaceBestTimeList, g_CarsOwnedList, g_CarsColorIDList, g_CarsDamageList, g_CarsIgNitroList, g_NumberRacesWon, g_Points, g_NumberCarsUnlocked, g_RacesNoDamage, g_MidAirTime

on validDomain
  if externalParamValue("src") starts "http:" then
    return 0
  end if
  validDomains = ["shockwave.com", "gallopinggames.com"]
  repeat with sDomain in validDomains
    if the moviePath contains sDomain then
      return 1
    end if
  end repeat
  return 0
end

on stopMovie
  if windowPresent("High Scores") then
    window("High Scores").close()
    window("High Scores").forget()
  end if
end

on startMovie
  if (the environment).runMode contains "Plugin" then
    the allowSaveLocal = 0
    tResult = 0
    if _player.runMode contains "Author" then
      tResult = 1
    else
      tResult = testSiteLock()
    end if
    if tResult = 0 then
      go("stolen")
    end if
  end if
  if g_FirstTime <> "FALSE" then
    g_FirstTime = "FALSE"
    g_SaveGameName = "RedlineRumble4SaveGame_214"
    g_NextFrame = 5
    g_PrevFrame = 5
    g_NextFramePlease = g_NextFrame
    g_MissionID = 0
    g_RaceID = 0
    g_CarsOwnedList = [2, 0, 0, 0, 0, 0]
    g_CarsColorIDList = [1, 2, 3, 4, 5, 1]
    g_CarsDamageList = [0, 0, 0, 0, 0, 0]
    g_CarsIgNitroList = [1, 1, 1, 1, 1, 1]
    g_MissionHiScoreList = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    g_RaceBestTimeList = [0, 0, 0, 0, 0, 0]
    tempResult = getPref(g_SaveGameName)
    if tempResult = VOID then
      tempSaveString = "[" & g_CarsOwnedList & "," & g_CarsColorIDList & "," & g_CarsDamageList & "," & g_CarsIgNitroList & "]"
      put "tempSaveString = " & tempSaveString
      setPref(g_SaveGameName, tempSaveString)
    else
      put "tempResult = " & tempResult
      tempLoadData = value(tempResult)
      put "tempLoadData = " & tempLoadData
      g_CarsOwnedList = tempLoadData[1]
      g_CarsColorIDList = tempLoadData[2]
      g_CarsDamageIDList = tempLoadData[3]
      g_CarsIgNitroList = tempLoadData[4]
    end if
    g_NumberRacesWon = 0
    g_Points = 0
    g_NumberCarsUnlocked = 0
    g_RacesNoDamage = 0
    g_MidAirTime = 0
  else
    g_NextFrame = 19
    g_PrevFrame = 19
    g_NextFramePlease = g_NextFrame
    g_NumberRacesWon = 0
    g_Points = 0
    g_NumberCarsUnlocked = 0
    g_RacesNoDamage = 0
    g_MidAirTime = 0
  end if
end
