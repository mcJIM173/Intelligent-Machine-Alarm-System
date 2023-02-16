from tracemalloc import start
from dash import Dash, dcc, callback_context
import dash_table
import dash_html_components as html
from dash.dependencies import Input, Output

import plotly.express as px

import pandas as pd
import pymysql
from datetime import date, datetime, timedelta

three_month = timedelta(days=180)
daydelta = timedelta(days=1)

db = pymysql.connect(
    host='127.0.0.1',
    port=3306,
    user='root',
    passwd='Fishcool/0227',
    db='ESP8266_test',
    charset='utf8'
)
cursor = db.cursor()
cursor.execute("SELECT * FROM MAS_STATUS")
data = cursor.fetchall()
data = list(data)
name_list = [tuple[0] for tuple in cursor.description]
data_comment = pd.DataFrame(data, columns=name_list)
db.close()
app = Dash(__name__)
app.layout = html.Div([
    html.H1('工業擬真診斷平台'),
    dcc.Dropdown(
        options=[
            {'label':'單日選擇', 'value':'single'}, 
            {'label':'多日選擇', 'value':'multi'}
        ], 
        value='single', 
        id='date_range'
    ),
    html.Div(id='date_select'),
    html.Div(id='status_text')
])


@app.callback(
    [Output('date_select', 'children'),
    Output('status_text', 'children')],
    Input('date_range', 'value')
)
def datePicker(value):
    if value == 'single':
        return (
            dcc.DatePickerSingle(
                id='data_date_picker_single',
                max_date_allowed=date.today(),
                min_date_allowed=date.today()-three_month,
                initial_visible_month=date.today(),
                date=date.today()
            ),
            html.Div(id='single_table')
        )
    elif value == 'multi':
        return (
            dcc.DatePickerRange(
                id='data_date_picker_range',
                max_date_allowed=date.today(),
                min_date_allowed=date.today()-three_month,
                initial_visible_month=date.today(),
                end_date=date.today(),
                start_date=date.today()-daydelta
            ),
            html.Div(id='multi_table')
        )


@app.callback(
    Output('multi_table', 'children'),
    Input('data_date_picker_range', 'start_date'),
    Input('data_date_picker_range', 'end_date'),
)
def show_table(start_date, end_date):
    tline_pd = []
    flame_pre = 0
    tilt_pre = 0
    power_pre = 0
    motor_pre = 0

    tilt = 0
    flame = 0
    power = 0

    start_date_object = date.fromisoformat(start_date)
    end_date_object = date.fromisoformat(end_date)
    flame_start = str(datetime.fromisoformat(start_date))
    tilt_start = str(datetime.fromisoformat(start_date))
    power_start = str(datetime.fromisoformat(start_date))
    motor_start = str(datetime.fromisoformat(start_date))

    db = pymysql.connect(
    host='127.0.0.1',
    port=3306,
    user='root',
    passwd='Fishcool/0227',
    db='ESP8266_test',
    charset='utf8'
    )
    cursor = db.cursor()
    cursor.execute("SELECT * FROM MAS_STATUS WHERE save_time >= '" + str(start_date_object) + "' AND save_time < '" + str(end_date_object + daydelta) + "'")
    data = cursor.fetchall()
    data = list(data)
    name_list = [tuple[0] for tuple in cursor.description]
    db.close()
    data_comment = pd.DataFrame(data, columns=name_list)
    if(data_comment.empty):
        return "沒有任何異常"
    else:
        for i in data_comment.index:
            if data_comment['flame'][i] == 1 and data_comment['flame'][i] != flame_pre:
                flame_start = data_comment['save_time'][i]
                flame_pre = 1
            elif data_comment['flame'][i] == 0 and data_comment['flame'][i] != flame_pre:
                flame_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='flame', Start=flame_start, Finish=flame_end))
                flame_pre = 0
            
            if data_comment['tilt'][i] == 1 and data_comment['tilt'][i] != tilt_pre:
                tilt_start = data_comment['save_time'][i]
                tilt_pre = 1
            elif data_comment['tilt'][i] == 0 and data_comment['tilt'][i] != tilt_pre:
                tilt_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='tilt', Start=tilt_start, Finish=tilt_end))
                tilt_pre = 0

            
            if data_comment['power'][i] == 1 and data_comment['power'][i] != power_pre:
                power_start = data_comment['save_time'][i]
                power_pre = 1
            elif data_comment['power'][i] == 0 and data_comment['power'][i] != power_pre:
                power_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='power', Start=power_start, Finish=power_end))
                power_pre = 0
            
            if data_comment['motor'][i] == 1 and data_comment['motor'][i] != motor_pre:
                motor_start = data_comment['save_time'][i]
                motor_pre = 1
            elif data_comment['motor'][i] == 0 and data_comment['motor'][i] != motor_pre:
                motor_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='motor', Start=motor_start, Finish=motor_end))
                motor_pre = 0
        
        if flame_pre == 1:
            flame_end = str(datetime.fromisoformat(end_date) + daydelta)
            tline_pd.append(dict(Status='flame', Start=flame_start, Finish=flame_end))
        if tilt_pre == 1:
            tilt_end = str(datetime.fromisoformat(end_date) + daydelta)
            tline_pd.append(dict(Status='tilt', Start=tilt_start, Finish=tilt_end))
        if power_pre == 1:
            power_end = str(datetime.fromisoformat(end_date) + daydelta)
            tline_pd.append(dict(Status='power', Start=power_start, Finish=power_end))
        if motor_pre == 1:
            motor_end = str(datetime.fromisoformat(end_date) + daydelta)
            tline_pd.append(dict(Status='motor', Start=motor_start, Finish=motor_end))
        
        fig = px.timeline(tline_pd, x_start='Start', x_end='Finish', y='Status', color='Status')
        return [
            "有異常！",
            dcc.Graph(id='timeline', figure=fig)
        ]


@app.callback(
    Output('single_table', 'children'),
    Input('data_date_picker_single', 'date')
)
def show_table(date_value):
    tline_pd = []
    flame_pre = 0
    tilt_pre = 0
    power_pre = 0
    motor_pre = 0

    date_object = date.fromisoformat(date_value)
    flame_start = str(datetime.fromisoformat(date_value))
    tilt_start = str(datetime.fromisoformat(date_value))
    power_start = str(datetime.fromisoformat(date_value))
    motor_start = str(datetime.fromisoformat(date_value))

    db = pymysql.connect(
    host='127.0.0.1',
    port=3306,
    user='root',
    passwd='Fishcool/0227',
    db='ESP8266_test',
    charset='utf8'
    )
    cursor = db.cursor()
    cursor.execute("SELECT * FROM MAS_STATUS WHERE save_time >= '" + str(date_object) + "' AND save_time < '" + str(date_object + daydelta) + "'")
    data = cursor.fetchall()
    data = list(data)
    name_list = [tuple[0] for tuple in cursor.description]
    db.close()
    data_comment = pd.DataFrame(data, columns=name_list)
    if(data_comment.empty):
        return "沒有任何異常"
    else:
        for i in data_comment.index:
            if data_comment['flame'][i] == 1 and data_comment['flame'][i] != flame_pre:
                flame_start = data_comment['save_time'][i]
                flame_pre = 1
            elif data_comment['flame'][i] == 0 and data_comment['flame'][i] != flame_pre:
                flame_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='flame', Start=flame_start, Finish=flame_end))
                flame_pre = 0
            
            if data_comment['tilt'][i] == 1 and data_comment['tilt'][i] != tilt_pre:
                tilt_start = data_comment['save_time'][i]
                tilt_pre = 1
            elif data_comment['tilt'][i] == 0 and data_comment['tilt'][i] != tilt_pre:
                tilt_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='tilt', Start=tilt_start, Finish=tilt_end))
                tilt_pre = 0

            
            if data_comment['power'][i] == 1 and data_comment['power'][i] != power_pre:
                power_start = data_comment['save_time'][i]
                power_pre = 1
            elif data_comment['power'][i] == 0 and data_comment['power'][i] != power_pre:
                power_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='power', Start=power_start, Finish=power_end))
                power_pre = 0
            
            if data_comment['motor'][i] == 1 and data_comment['motor'][i] != motor_pre:
                motor_start = data_comment['save_time'][i]
                motor_pre = 1
            elif data_comment['motor'][i] == 0 and data_comment['motor'][i] != motor_pre:
                motor_end = data_comment['save_time'][i]
                tline_pd.append(dict(Status='motor', Start=motor_start, Finish=motor_end))
                motor_pre = 0
        
        if flame_pre == 1:
            if date_object != date.today():
                flame_end = str(datetime.fromisoformat(date_value) + daydelta)
            else:
                flame_end = str(datetime.now())
            tline_pd.append(dict(Status='flame', Start=flame_start, Finish=flame_end))
        if tilt_pre == 1:
            if date_object != date.today():
                tilt_end = str(datetime.fromisoformat(date_value) + daydelta)
            else:
                tilt_end = str(datetime.now())
            tline_pd.append(dict(Status='tilt', Start=tilt_start, Finish=tilt_end))
        if power_pre == 1:
            if date_object != date.today():
                power_end = str(datetime.fromisoformat(date_value) + daydelta)
            else:
                power_end = str(datetime.now())
            tline_pd.append(dict(Status='power', Start=power_start, Finish=power_end))
        if motor_pre == 1:
            if date_object != date.today():
                motor_end = str(datetime.fromisoformat(date_value) + daydelta)
            else:
                motor_end = str(datetime.now())
            tline_pd.append(dict(Status='motor', Start=motor_start, Finish=motor_end))
        
        fig = px.timeline(tline_pd, x_start='Start', x_end='Finish', y='Status', color='Status')

        return [
            "有異常！",
            dcc.Graph(id='timeline', figure=fig)
        ]


if __name__ == '__main__':
    app.run_server(debug=True)