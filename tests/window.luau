local window = std.gui.window("my window", vector.create(800, 600, 0))
print(window.size)
print(window.title)

window.title = "Yo"
window.size = vector.create(600, 400, 0)

