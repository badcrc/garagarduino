#!/usr/bin/env python

#based on http://coreygoldberg.blogspot.com.es/2012/04/python-getting-data-into-graphite-code.html

import socket
import time
import datetime
import csv

def send_to_carbon(metric_path,value,timestamp):
    CARBON_SERVER = ''
    CARBON_PORT = 2003

    message = '%s %s %s\n' % (metric_path,value,timestamp)

    #print 'sending message:\n%s' % message
    sock = socket.socket()
    sock.connect((CARBON_SERVER, CARBON_PORT))
    sock.sendall(message)
    sock.close()

def main():
	
	temp_metric="stats.gauges.Homebrew.temperature"
	hot_metric="stats.gauges.Homebrew.hot"
	cold_metric="stats.gauges.Homebrew.cold"
	
	with open('DATALOG.CSV', 'rb') as csvfile:
	    file_data = csv.reader(csvfile, delimiter=',')
	    
	    
            for row in file_data:
                date = time.mktime(datetime.datetime.strptime(row[0], "%Y-%m-%d %H:%M:%S").timetuple())
                temperature=row[1]
                hot = row[2]
                cold = row[3]
                
                #we don't want tests sent to graphite
                if float(temperature) > 50.00: 
                    continue
                
                send_to_carbon(temp_metric,temperature,date)
                send_to_carbon(hot_metric,hot,date)
                send_to_carbon(cold_metric,cold,date)


if __name__ == '__main__':
    main()


