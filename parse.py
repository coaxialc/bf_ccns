import os

results=open("results.txt", "r")

r=[]
#r2=[]

for line in results:       
        r.append(line.split())  
    
#print(r)

#lista poy se kathe thesi exei ton meso oro tou plithous ton interests
#gia ena group size
interests = {}

#lista poy se kathe thesi exei ton meso oro tou plithous ton data
#gia ena group size
data = {}

#to check when size changes from one size to another
previousSize=5

interestSum=0
dataSum=0

for line in r:
    group_size = int(line[3])
    interest_list = interests.get(group_size)
    if not interest_list:
       interest_list = []
       interests[group_size] = interest_list

    interest_list.append(int(line[7]))

    data_list = data.get(group_size)
    if not data_list:
       data_list = []
       data[group_size] = data_list

    data_list.append(int(line[9]))

#print(interestAverage)
#print(dataAverage)
'''
for i in range(0, (len(interestAverage))*5+1,5):
    for j in range(0,10):
        path=os.path.join('./pit_stats','gs'+str(i)+'experiment'+str(j)+'.txt')
        if(os.path.isfile(path)):
            with open(path, 'r') as file2:
                r2.append(file2.readline())

print(r2)
print('length')
print(len(r2))
    '''
wf=open('gs_data.txt', 'w')

groups = list(data.keys())
groups.sort()
for group in groups:
     data_list = data[group]
     s = sum(data_list)
     data_avg = float(s/float(len(data_list)))
     wf.write('%s %f\n'% (str(group), data_avg))
     
wf.flush()    
wf.close()

wf2=open('gs_interests.txt', 'w')

groups = list(interests.keys())
groups.sort()
for group in groups:
     interest_list = interests[group]
     s = sum(interest_list)
     interests_avg = float(s/float(len(interest_list)))
     wf2.write('%s %f\n'% (str(group), interests_avg))

wf2.flush()
wf2.close()


sizes=[]
s=[]
path = r'./pit_stats'


for file in os.listdir(path):
    file_path = os.path.join(path, file)
    if os.path.isfile(file_path):
        with open(file_path, 'r') as afile:
             sum=int(0)
	     lineCount=int(0)
           
             for line in afile:       
                sum=sum+int(line.split()[-1]) 
		lineCount=lineCount+1

	     sum=sum/lineCount
              
             if file.split("-")[1] in sizes:
		s[sizes.index(file.split("-")[1])]=s[sizes.index(file.split("-")[1])]+sum		
             else:
                 sizes.append(file.split("-")[1])
                 s.append(sum)
            

avg=[]

for i in s:
    avg.append(float(i)/10)

for i in sizes:
    print(i)


for i in avg:
    print(i)

sizes = map(float, sizes)
avg = map(float, avg)

avg=[x for (y,x) in sorted(zip(sizes,avg))]
sizes.sort()

print(sizes)
print(avg)

ww=open('gs_avgpit.txt', 'w')


for i in range(0,len(sizes)):
     ww.write('%f %f\n'% (sizes[i],avg[i]))


ww.flush()
ww.close()


