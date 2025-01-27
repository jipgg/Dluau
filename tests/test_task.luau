local time = require('@require/qol2/time')
print.text_color = 'blue'

task.defer(function()
    print("bye")
end)
task.spawn(function()
    print('yooo')
    task.wait()
    task.defer(function()
        print("deferred")
    end)
end)
task.wait(2)
task.defer(function()
    print("byeeeeeeeeeeeee")
end)
print("fr")

