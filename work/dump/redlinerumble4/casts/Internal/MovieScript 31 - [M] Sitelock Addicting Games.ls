global showSiteLock

on AGSitelock me
  tState = validDomainAddictingGames()
  return tState
end

on validDomainAddictingGames
  validDomains = ["xform.nl", "addictinggames.com", "shockwave.com", "mtvi.com"]
  lPath = externalParamValue("src")
  if showSiteLock = 1 then
    put lPath
  end if
  lIdx = 3
  if not (lPath starts "http") then
    lPath = the moviePath
  end if
  if showSiteLock = 1 then
    put lPath
  end if
  lSplitRes1 = Split(lPath, "/")
  if showSiteLock = 1 then
    put lSplitRes1
  end if
  lTest = 0
  if lSplitRes1.count >= lIdx then
    repeat with sDomain in validDomains
      if StringEnds(lSplitRes1[lIdx], sDomain) then
        lTest = 1
        exit repeat
      end if
    end repeat
  end if
  if lTest then
    validBrowserDomain(validDomains)
  end if
  return lTest
end

on Split tString, tDelimiter
  the itemDelimiter = tDelimiter
  tList = []
  repeat with i = 1 to tString.item.count
    tEntry = tString.item[i]
    tList.add(tEntry)
  end repeat
  return tList
end

on StringEnds tString1, tString2
  if showSiteLock = 1 then
    put tString1 && tString2
  end if
  tState = 1
  tL1 = tString1.length
  tL2 = tString2.length
  tDiff = abs(tL1 - tL2)
  if showSiteLock = 1 then
    put tL1 && tL2 && tDiff
  end if
  repeat with i = tL1 down to tDiff
    tChar1 = tString1.char[i]
    if (i - tDiff) = 0 then
      exit repeat
    end if
    tChar2 = tString2.char[i - tDiff]
    if showSiteLock = 1 then
      put tChar1 && tChar2
    end if
    if tChar1 <> tChar2 then
      return 0
    end if
  end repeat
  return tState
end

on validBrowserDomain kValidDomains
  validDomains = kValidDomains
  lGuard = "true"
  repeat with sDomain in validDomains
    lGuard = lGuard & " && (!(document.location.host.indexOf('" & sDomain & "') !== -1))"
  end repeat
  lStatement = "if(" & lGuard & ") document.location.href = 'http://www." & kValidDomains[1] & "'"
  gotoNetPage("javascript: " & lStatement)
  if showSiteLock = 1 then
    put lStatement
  end if
end
