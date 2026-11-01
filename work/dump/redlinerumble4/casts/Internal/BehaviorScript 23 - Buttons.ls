property nextFrame, this, startWidth, startHeight
global g_NextFrame

on beginSprite
  nextFrame = integer(sprite(the currentSpriteNum).member.name.item[1])
  this = the currentSpriteNum
  startWidth = sprite(the currentSpriteNum).member.width
  startHeight = sprite(the currentSpriteNum).member.height
end

on exitFrame
  if the rollover = this then
    if the mouseDown then
      sprite(the currentSpriteNum).width = startWidth * 0.80000000000000004
      sprite(the currentSpriteNum).height = startHeight * 0.80000000000000004
    else
      sprite(the currentSpriteNum).width = startWidth * 1.0
      sprite(the currentSpriteNum).height = startHeight * 1.0
    end if
    sprite(the currentSpriteNum).blend = 100
  else
    sprite(the currentSpriteNum).blend = 50
    sprite(the currentSpriteNum).width = startWidth * 1.0
    sprite(the currentSpriteNum).height = startHeight * 1.0
  end if
end

on mouseUp
  g_NextFrame = nextFrame
end
