from bluepy.btle import Peripheral, UUID
from bluepy.btle import Scanner, DefaultDelegate

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
    def handleDiscovery(self,dev,isNewDev,isNewData):
        if isNewDev:
            print('Discovered Device: ',dev.addr)
        elif isNewData:
            print('Received new data from: ',dev.addr)
    def handleNotification(self,cHandle,data):
        print(str(data))

def enable_notify(dev,chara_uuid):
    setup_data = b"\x01\x00"
    notify = dev.getCharacteristics(uuid=chara_uuid)[0]
    notify_handle = notify.getHandle() + 1
    dev.writeCharacteristic(notify_handle, setup_data, withResponse=True)

scanner=Scanner().withDelegate(ScanDelegate())
devices=scanner.scan(5.0)
n=0
for dev in devices:
    print('%d: Device %s (%s), RSSI=%d dB' %(n,dev.addr,dev.addrType,dev.rssi))
    n+=1
    for(adtype,desc,value) in dev.getScanData():
        print("%s=%s" %(desc,value))
number=input('Enter your device number: ')
print('Device',number)
print(devices[number].addr)

print('Connecting...')
dev=Peripheral(devices[number].addr,'random')
#print('Services...')
#for svc in dev.services:
    #print(str(svc))

try:
    #testService=dev.getServiceByUUID(UUID(0xa004))
    #for ch in testService.getCharacteristics():
    #print(str(ch))
    ch=dev.getCharacteristics(uuid=UUID(0xa005))[0]
    if(ch.supportsRead()):
        print(ch.read())

    for i in range(10):
        enable_notify(dev,0xa001)
        if dev.waitForNotifications(5.0):
            # handleNotification() was called
            print("Button pressed")
            continue

        print "Waiting..."
        # Perhaps do something else here

finally:
    dev.disconnect()