# coding=utf-8

import xlwt
import os

user_need_file = 'temp_log.log'

if __name__ == '__main__':
	cur_file_pwd = os.getcwd()
	print(cur_file_pwd)

	p_excel_file = xlwt.Workbook(encoding='utf-8',style_compression=0)
	p_excel_sheet = p_excel_file.add_sheet('data')
	line_count = 0
	p_file = open("f:\\temp_code\\write_data_to_excel\\temp_log.log", "r")
	p_line = p_file.readlines()
	for line in p_line:
		if "<00> info> app: Temp" in line :
			p_excel_sheet.write(line_count, 0, line[-7:-2])
			line_count += 1
	print(line_count)
	p_excel_file.save("temp.xls")
			
		
