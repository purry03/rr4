property imagePanel, texturePanel, panelCounter, panelLoc, maskPanel, panelUP, panelTarget, panelLayer, imageKeyPanel, keyPanelLayer, textureKeyPanel, panelBlend, panelDownTicks, cam, prevMessageID
global gData, gPaused

on beginSprite
  w = sprite(the currentSpriteNum).member
  cam = sprite(the currentSpriteNum).camera
  imagePanel = member(21, "Textures").image.duplicate()
  texturePanel = w.newTexture("PanelOverlayTexture", #fromImageObject, imagePanel)
  texturePanel.renderFormat = #rgba8888
  cam.addOverlay(texturePanel, point(278, 9), 0)
  panelLayer = cam.overlay.count
  panelLoc = cam.overlay[panelLayer].loc
  cam.overlay[panelLayer].blend = 0
  panelBlend = 0
  panelCounter = 0
  panelUP = 1
  panelTarget = 469
  doPanel(0, rgb(0, 0, 0))
  prevMessageID = 0
  panelDownTicks = 0
  doPanel(0, 10, rgb(255, 255, 255), 1000)
end

on exitFrame
  if panelDownTicks > 0 then
    if the ticks >= panelDownTicks then
      panelUP = 0
    end if
  end if
  if gPaused = 0 then
    if panelUP then
      if panelBlend < 70 then
        panelBlend = panelBlend + 5
      end if
      cam.overlay[panelLayer].blend = 100
      cam.overlay[panelLayer].scale = panelBlend / 70.0
    else
      if panelBlend > 0 then
        panelBlend = panelBlend - 5
      end if
      cam.overlay[panelLayer].blend = panelBlend
      cam.overlay[panelLayer].scale = panelBlend / 70.0
    end if
  end if
end

on doPanel me, messageID, messageColor, messageTicks
  if messageTicks > 0 then
    panelDownTicks = the ticks + messageTicks
  else
    panelDownTicks = 0
  end if
  if prevMessageID <> messageID then
    prevMessageID = messageID
    if messageID > 0 then
      cam.overlay[panelLayer].scale = 0
      panelBlend = 0
      cam.overlay[panelLayer].blend = panelBlend
    end if
  end if
  if messageID > 0 then
    panelUP = 1
    tempAlpha = member(messageID, "infoPanels").image.extractAlpha()
    tempMask = tempAlpha.createMask()
    tempFill = image(tempAlpha.width, tempAlpha.height, 8)
    tempFill.fill(tempFill.rect, messageColor)
    tempFill.fill(rect(0, 0, tempFill.width, 12), messageColor)
    imagePanel = member(21, "Textures").image.duplicate()
    imagePanel.copyPixels(tempFill, tempFill.rect + rect(5, 20, 5, 20), tempFill.rect, [#maskImage: tempMask])
    texturePanel.image = imagePanel
    tempFill = 0
    tempMask = 0
    tempAlpha = 0
  else
    panelUP = 0
  end if
end
