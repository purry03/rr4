property spriteNum

on mouseEnter me
  _player.cursor(280)
end

on mouseLeave me
  _player.cursor(-1)
end

on mouseUp
  gotoNetPage("http://www.addictinggames.com", "_new")
end

on exitFrame me
  if sprite(spriteNum).playing then
    go(the frame)
  else
    go(5)
  end if
end
