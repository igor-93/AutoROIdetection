from os import listdir, walk, remove, rename, system
from os.path import isfile, join
directory = '/home/igorpesic/workspace/OMABS/trunk/OMABS/images/'
#dst = '/home/igorpesic/workspace/OralHygiene/OralHygiene/trunk/OralHygiene/images/all/'
wanted_endings = ['.jpg', '.png', '.bmp']
end_len = len(wanted_endings[2])
total = 0
curr = 0
for item in [x[0] for x in walk(directory)]:
    #print item
    onlyfiles = [join(item, f) for f in listdir(item)[:1] if isfile(join(item, f))]
    total += len(onlyfiles)
    for file_ in onlyfiles:
        if file_[-end_len:] in wanted_endings:
            curr += 1
            args = file_+' '+str(curr)
            print args
            #print dst+file_.split('/')[-1]
            system("/home/igorpesic/ClionProjects/AutoROIdetection/Debug/AutoROIdetection " + args)
            
print 'total ', total