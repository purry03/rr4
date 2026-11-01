on beginSprite
end

on exitFrame
  sprite(the currentSpriteNum).locV = sprite(the currentSpriteNum).locV - 1
end
