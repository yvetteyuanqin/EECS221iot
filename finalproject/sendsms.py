import requests

def sms_alert():#first, second, third):
    '''report = {}
    report["value1"] = first
    report["value2"] = second
    report["value3"] = third
    '''
    requests.post("https://maker.ifttt.com/trigger/Abnormality_or_Intruder/with/key/cfjADuIP82aALj5vEQdZuR")#, data=report)    
'''
print("Choose your first string.")
a = input()
print("Choose your second string.")
b = input()
print("Choose your third string.")
c = input()
'''
sms_alert()#a, b, c)
