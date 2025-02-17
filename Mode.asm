;counting how many times a number is repeated in an array
array:  data 2
        data 1
        data 2
        data 4
        data 2
size:   data 5     
target: SET 2 
count:  data 0
ldc array
a2sp
ldc 1
loop:
brz end
ldl 0
ldc target
sub
brz increment
skip:
    adj 1
    ldc size
    ldnl 0
    adc -1
    ldc size
    stnl 0
    ldnl 0
    br loop
increment:
    ldc count
    ldnl 0
    adc 1
    ldc count    
    stnl 0
    br skip
end:
    HALT