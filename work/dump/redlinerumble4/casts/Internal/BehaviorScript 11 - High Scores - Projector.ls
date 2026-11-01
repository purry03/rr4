property pFunction, pScoreMem, pGameID, pTitle1, pScreenW, pScreenH, pMarker, pScoreDes, pSuccMarker, pFailMarker, oState
global gSwScoreSetupList, gSwScoreNetID, lastLabel, gCloseHS, gStartGameFrame, gNextScene, gPrevScene, g_NumberRacesWon, g_Points, g_NumberCarsUnlocked, g_RacesNoDamage, g_MidAirTime

on getPropertyDescriptionList me
  return [#pFunction: [#comment: "Submit / View Button?", #format: #symbol, #range: [#submit, #View], #default: #submit], #pScoreMem: [#comment: "Field or text member holding score:", #format: #member, #default: EMPTY], #pGameID: [#comment: "Game ID:", #format: #string, #default: EMPTY], #pTitle1: [#comment: "Game Title:", #format: #string, #default: EMPTY], #pScreenW: [#comment: "Screen Width", #format: #integer, #default: (the stage).rect.width], #pScreenH: [#comment: "Screen Height", #format: #integer, #default: (the stage).rect.height], #pMarker: [#comment: "Marker to wait at while HS Hub downloads:", #format: #marker, #default: EMPTY], #pScoreDes: [#comment: "Score Descriptor (points, kills, etc.)", #format: #string, #default: "points"]]
end

on getBehaviorDescription
  return "DESCRIPTION:" & RETURN & "This is the only behavior required for high score setup. It will configure your movie for Name, Score scores display." & RETURN & "Additional optional behaviors are required if you want to use more fields or have multiple players." & RETURN & RETURN & "PARAMETERS:" & RETURN & "• Submit Scores Button / View Scores Button: Which button is this behavior on?" & RETURN & "• Text member holding Score: The text member holding the score." & RETURN & "• Game ID: Unique ID provided to you by Macromedia." & RETURN & "• Game Title: Title to display in the HS Hub." & RETURN & "• Screen Width: Width of your game's screen." & RETURN & "• Screen Height: Height of your game's screen." & RETURN & "• Marker to wait at while HS Hub downloads: set up this marker in your game." & RETURN & "• Score descriptor: How would you like your score labeled?" & RETURN & RETURN & "-- © kwik 2000"
end

on beginSprite
  oState = #WAITING
end

on exitFrame me
  case oState of
    #loading:
      if netDone(gSwScoreNetID[1]) then
        if netError(gSwScoreNetID[1]) = "OK" then
          openWindowHS(me)
          oState = #hsopen
        else
          oState = #error
        end if
      else
        myStatus = getStreamStatus(gSwScoreNetID[1])
        if myStatus[#bytesTotal] > 0 then
          myPct = float(myStatus[#bytesSoFar]) / float(myStatus[#bytesTotal]) * 100
        else
          myPct = 0
        end if
      end if
    #hsopen:
      if gCloseHS then
        if windowPresent("High Scores") then
          window("High Scores").close()
          window("High Scores").forget()
        end if
        if count(the windowList) > 0 then
          window("High Scores").forget()
        end if
        oState = #done
      end if
    #done:
    #error:
      sendSprite(1, #submitFailed)
      oState = #WAITING
  end case
end

on openWindowHS me
  window("High Scores").fileName = gSwScoreNetID[2]
  window("High Scores").rect = (the stage).rect
  window("High Scores").windowtype = 2
  window("High Scores").open()
  window("High Scores").moveToFront()
end

on InitList me, paramGameID, paramGameTitle, paramScoreDes
  put "InitList" && paramGameID && paramGameTitle && paramScoreDes
  if not listp(gSwScoreSetupList) then
    gSwScoreSetupList = [:]
    sort(gSwScoreSetupList)
  end if
  addToList(me, #gameid, paramGameID)
  addToList(me, #gametitle1, paramGameTitle)
  addToList(me, #gametitle2, EMPTY)
  addToList(me, #width, pScreenW)
  addToList(me, #height, pScreenH)
  addToList(me, #userName, EMPTY)
  addToList(me, #userscore, pScoreMem.text)
  addToList(me, #scoredesc, paramScoreDes)
  addToList(me, #movielocation, the moviePath & the movieName)
  addToList(me, #optional1, [])
  addToList(me, #optional2, [])
  addToList(me, #optional3, [])
  addToList(me, #hexes, [])
  addToList(me, #Function, pFunction)
  addToList(me, #status, #start)
  addToList(me, #NumberRacesWon, string(g_NumberRacesWon))
  addToList(me, #Points, string(g_Points))
  addToList(me, #NumberCarsUnlocked, string(g_NumberCarsUnlocked))
  addToList(me, #RacesNoDamage, string(g_RacesNoDamage))
  addToList(me, #MidAirTime, string(g_MidAirTime))
  put gSwScoreSetupList
end

on addToList me, whatProp, whatValue
  if voidp(getaProp(gSwScoreSetupList, whatProp)) then
    addProp(gSwScoreSetupList, whatProp, whatValue)
  else
    setaProp(gSwScoreSetupList, whatProp, whatValue)
  end if
end

on branchToScores me
  pSuccMarker = 5
  pFailMarker = 5
  case (the environment).runMode of
    "Plugin":
      if (the environment).shockMachine = 1 then
        myURL = setupGlobalBranch(me)
      else
        if the moviePath contains "http://" then
          myURL = setupGlobalBranch(me)
        else
          myURL = EMPTY
        end if
      end if
      gSwFromHS = 1
      gSwScoreNetID = [gotoNetMovie(myURL), myURL]
    "Projector", "Author":
      gSwFromHS = 1
      myURL = "http://www.shockwave.com/content/highscores/nosave/hshub-2000-2.dcr"
      gSwScoreNetID = [preloadNetThing(myURL), myURL, pSuccMarker, pFailMarker]
      gCloseHS = 0
      oState = #loading
  end case
end

on setupGlobalBranch me
  if not voidp(externalParamValue("swURL")) then
    myURL = externalParamValue("swURL")
  else
    myGameURL = the moviePath
    the itemDelimiter = "/"
    mySite = item 3 of myGameURL
    if (myGameURL contains "http://") and (mySite contains "shockwave.com") then
      the itemDelimiter = "."
      myPrefix = item 1 of mySite
      myURL = "http://" & myPrefix & ".shockwave.com/content/highscores/scorez-2000.dcr"
    else
      myURL = "http://www.shockwave.com/content/highscores/scorez-2000.dcr"
    end if
  end if
  return myURL
end
