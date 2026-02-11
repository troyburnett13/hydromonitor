 #!/usr/bin/python3


#################################################################################################################################################
#                                                    CLASSES CONTAINING ALL THE APP FUNCTIONS                                                                                                    #
#################################################################################################################################################


class DB:

    def __init__(self,Config):

        from math import floor
        from os import getcwd
        from os.path import join
        from json import loads, dumps, dump
        from datetime import timedelta, datetime, timezone 
        from pymongo import MongoClient , errors, ReturnDocument
        from urllib import parse
        from urllib.request import  urlopen 
        from bson.objectid import ObjectId  
       
      
        self.Config                         = Config
        self.getcwd                         = getcwd
        self.join                           = join 
        self.floor                      	= floor 
        self.loads                      	= loads
        self.dumps                      	= dumps
        self.dump                       	= dump  
        self.datetime                       = datetime
        self.ObjectId                       = ObjectId 
        self.server			                = Config.DB_SERVER
        self.port			                = Config.DB_PORT
        # Safely encode credentials; fall back to empty string to avoid TypeErrors when env vars are missing
        self.username                   	= parse.quote_plus(Config.DB_USERNAME or "")
        self.password                   	= parse.quote_plus(Config.DB_PASSWORD or "")
        self.remoteMongo                	= MongoClient
        self.ReturnDocument                 = ReturnDocument
        self.PyMongoError               	= errors.PyMongoError
        self.BulkWriteError             	= errors.BulkWriteError  
        # Default to no TLS unless explicitly enabled in Config
        self.tls                            = getattr(Config, "DB_TLS", False)
        # Build Mongo URI with or without credentials depending on availability
        if self.username and self.password:
            self.mongo_uri = f"mongodb://{self.username}:{self.password}@{self.server}:{self.port}"
        else:
            self.mongo_uri = f"mongodb://{self.server}:{self.port}"


    def __del__(self):
            # Delete class instance to free resources
            pass
 


    ####################
    # LAB 2 DATABASE UTIL FUNCTIONS  #
    ####################
    
    def addUpdate(self,data):
        '''ADD A NEW STORAGE LOCATION TO COLLECTION'''
        try:
            remotedb 	= self.remoteMongo(self.mongo_uri, tls=self.tls)
            result      = remotedb.ELET2415.climo.insert_one(data)
            inserted = result.inserted_id
            print(f"Mongo insert OK id={inserted}")
            return inserted
        except Exception as e:
            msg = str(e)
            if "duplicate" not in msg.lower():
                print(f"addUpdate error [{self.mongo_uri}] ",msg)
            return None
        
       

    def getAllInRange(self,start, end):
        '''RETURNS A LIST OF OBJECTS. THAT FALLS WITHIN THE START AND END DATE RANGE'''
        try:
            remotedb 	= self.remoteMongo(self.mongo_uri, tls=self.tls)
            result      = list(remotedb.ELET2415.climo.find({"timestamp":{"$gte":int(start),"$lte":int(end)}}, {"_id":0}).sort("timestamp",1))
            #print(result)
            #print(end)
            return result
        except Exception as e:
            msg = str(e)
            print("getAllInRange error ",msg)            
            return []
        

    def humidityMMAR(self,start, end):
        '''RETURNS MIN, MAX, AVG AND RANGE FOR HUMIDITY. THAT FALLS WITHIN THE START AND END DATE RANGE'''
        try:
            remotedb 	= self.remoteMongo(self.mongo_uri, tls=self.tls)
            result      = list(remotedb.ELET2415.climo.aggregate([{"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}}, {"$group": {"_id": None, "humidity": {"$push": "$$ROOT.humidity"}}}, {"$project": {"_id": 0, "max": {"$max": "$humidity"}, "min": {"$min": "$humidity"},"avg": {"$avg": "$humidity"}, "range": {"$subtract": [{"$max": "$humidity"}, {"$min": "$humidity"}]}}}]))
            return result
        except Exception as e:
            msg = str(e)
            print("humidityMMAS error ",msg)            
            return []
        
    def temperatureMMAR(self,start, end):
        '''RETURNS MIN, MAX, AVG AND RANGE FOR TEMPERATURE. THAT FALLS WITHIN THE START AND END DATE RANGE'''
        try:
            remotedb 	= self.remoteMongo(self.mongo_uri, tls=self.tls)
            result      = list(remotedb.ELET2415.climo.aggregate([{"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}}, {"$group": {"_id": None, "temperature": {"$push": "$$ROOT.temperature"}}}, {"$project": {"_id": 0, "max": {"$max": "$temperature"}, "min": {"$min": "$temperature"},"avg": {"$avg": "$temperature"}, "range": {"$subtract": [{"$max": "$temperature"}, {"$min": "$temperature"}]}}}]))
            return result
        except Exception as e:
            msg = str(e)
            print("temperatureMMAS error ",msg)            
            return []


    def frequencyDistro(self,variable,start, end):
        '''RETURNS THE FREQUENCY DISTROBUTION FOR A SPECIFIED VARIABLE WITHIN THE START AND END DATE RANGE'''
        try:
            remotedb 	= self.remoteMongo(self.mongo_uri, tls=self.tls)
            result      = list(remotedb.ELET2415.climo.aggregate([{"$match": {"timestamp": {"$gte": int(start), "$lte": int(end)}}}, {"$bucket": {"groupBy": "$" + variable, "boundaries": [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100], "default": "outliers", "output": {"count": {"$sum": 1}}}}]))
            return result
        except Exception as e:
            msg = str(e)
            print("frequencyDistro error ",msg)            
            return []
        
 



def main():
    from config import Config
    from time import time, ctime, sleep
    from math import floor
    from datetime import datetime, timedelta
    one = DB(Config)
 
 
    start = time() 
    end = time()
    print(f"completed in: {end - start} seconds")
    
if __name__ == '__main__':
    main()


    
