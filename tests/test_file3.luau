task.wait(20)
print.text_color = "cyan"
local now = require("@require/qol2/time").now()
print(now:format('%X'))
print.reset_colors()
print(`{meta.current_script}: test hello world`)
