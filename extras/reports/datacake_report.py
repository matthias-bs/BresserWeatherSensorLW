###################################################################################################
# datacake_report-2.py
#
# This script generates a PDF report from CSV files containing sensor data
# which are provided by Datacake.
#
# created: 08/2024
#
#
# MIT License
#
# Copyright (c) 2024 Matthias Prinke
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#
# History:
#
# 20240901 Created
#
# ToDo:
# -
###################################################################################################

import os
import pandas as pd
#import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.backends.backend_pdf import PdfPages

LOCATION = '<Location>'

# Set the desired language
LANG = 'en'

# Directory containing the CSV files
src_dir = "datacake_csv"

# Output PDF file
pdf_file = "weather_report.pdf"

# Define the last column in the CSV files
last_column = 11

COLUMNS = {
    'ws_temp_c': 0,
    'ws_humidity': 6,
    'rain_mm': 8,
    'wind_avg': 5,
    'wind_gust': 9,
    'wind_direction': 10,
    'battery_voltage': 11,
}

COLORS = {
    'ws_temp_c': 'r',
    'ws_humidity': 'skyblue',
    'rain_mm': 'b',
    'wind_avg': 'g',
    'wind_gust': 'lime',
    'wind_direction': 'gray',
    'battery_voltage': 'orange',
}


# Define translations
translations = {
    'en': {
        'Weather Report': 'Weather Report',
        'Annual Overview': 'Annual Overview',
        'Air Temperature and Humidity': 'Air Temperature and Humidity',
        'Temperature [°C]': 'Temperature [°C]',
        'Relative Humidity [%]': 'Relative Humidity [%]',
        'Daily Avg.': 'Daily Avg.',
        'Rain': 'Rain',
        'Rain [mm]': 'Rain [mm]',
        'Rain [mm] (Rain Gauge)': 'Rain [mm] (Rain Gauge)',
        'Wind': 'Wind',
        'Average [m/s]': 'Average [m/s]',
        'Gusts [m/s]': 'Gusts [m/s]',
        'Direction [°]': 'Direction [°]',
        'Battery Voltage': 'Battery Voltage',
        'Voltage [mV]': 'Voltage [mV]',
        'Monthly Reports': 'Monthly Reports'
    },
    'de': {
        'Weather Report': 'Wetterbericht',
        'Annual Overview': 'Jahresübersicht',
        'Air Temperature and Humidity': 'Luftemperatur und -feuchte',
        'Temperature [°C]': 'Temperatur [°C]',
        'Relative Humidity [%]': 'rel. Feuchte [%]',
        'Daily Avg.': 'tägl. Durchschnitt',
        'Rain': 'Regen',
        'Rain [mm]': 'Regen [mm]',
        'Rain [mm] (Rain Gauge)': 'Regen [mm] (Regenmesser)',
        'Wind': 'Wind',
        'Average [m/s]': 'Durchschnitt [m/s]',
        'Gusts [m/s]': 'Böen [m/s]',
        'Direction [°]': 'Richtung [°]',
        'Battery Voltage': 'Batteriespannung',
        'Voltage [mV]': 'Spannung [mV]',
        'Monthly Reports': 'Monatsberichte'
    }
}

def tr(text):
    return translations.get(LANG, {}).get(text, text)


# List all CSV files in the directory
csv_files = [f for f in os.listdir(src_dir) if f.endswith('.csv')]

# Initialize an empty list to store DataFrames
dataframes = []

# Read each CSV file
for file in csv_files:
    file_path = os.path.join(src_dir, file)
    df = pd.read_csv(file_path, skiprows=0, parse_dates=[0], date_parser=lambda x: pd.to_datetime(x, format='%a, %d %b %Y %H:%M:%S'))
    
    for col in range(1, last_column):
        # Replace comma with dot and convert to float
        df.iloc[:, col] = df.iloc[:, col].astype(float)

    dataframes.append(df)

# Combine all DataFrames into a single DataFrame
combined_df = pd.concat(dataframes, ignore_index=True)

# Set the first column as the index (time)
combined_df.set_index(combined_df.columns[0], inplace=True)

# Sort the DataFrame by the index (time)
combined_df.sort_index(inplace=True)

# Print the combined DataFrame
print(combined_df)

# Remove invalid rows from the combined DataFrame
combined_df.dropna(inplace=True)

# Drop rows where column 3 (Humidity) is zero
combined_df = combined_df[combined_df.iloc[:, COLUMNS['ws_humidity']] != 0]


def title_page(plt, pdf, title, font_size=24):
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.text(0.5, 0.5, title, transform=ax.transAxes, fontsize=font_size, ha='center', va='center')
    ax.axis('off')
    pdf.savefig(fig)
    plt.close(fig)

def plot_data(plt, pdf, df, columns, title, xlabel, ylabels, colors, avg_label, avg_colors):
    fig, axes = plt.subplots(len(columns), 1, figsize=(12, 6), layout="tight")
    for i in range(len(columns)):
        if len(columns) == 1:
            ax = axes
        else:
            ax = axes[i]
        ax.plot(df.index, df.iloc[:, columns[i]], label=ylabels[i], color=colors[i])
        if xlabel:
            ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabels[i])
        #ax.tick_params(axis='y', labelcolor=colors[i])
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax.xaxis.set_major_locator(mdates.AutoDateLocator())
        if i < len(columns) - 1:
            # Remove x-axis labels for all but the last subplot
            ax.set_xticklabels([])
        if avg_colors[i]:
            daily_avg = df.iloc[:, columns[i]].resample('D').mean()
            ax.plot(daily_avg.index, daily_avg, label=f'{avg_label} {ylabels[i]}', color=avg_colors[i], linestyle='--', linewidth=2)

        ax.legend()
        ax.grid(True)
        
        # Rotate x-axis labels for better readability
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)

    fig.suptitle(title, fontsize=16)

    pdf.savefig(fig)
    plt.close(fig)

# Create a new figure for Rain Gauge
def plot_rain(plt, pdf, df_diff, df, column, title, xlabel, ylabel, color):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6), layout="tight")

    # Plot the Rain Gauge differences as a bar graph
    ax1.bar(df_diff.index, df_diff, label=ylabel[0], color=color)
    if xlabel:
        ax1.set_xlabel(xlabel)
    ax1.set_ylabel(ylabel[0])
    #ax.tick_params(axis='y', labelcolor='b')
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax1.xaxis.set_major_locator(mdates.AutoDateLocator())
    #ax1.set_xticklabels([])
    
    ax1.legend()
    ax1.grid(True)

    # Plot raw rain gauge value on the second subplot
    ax2.plot(df.index, df.iloc[:, column], label=ylabel[1], color=color)
    if xlabel:
        ax2.set_xlabel(xlabel)
    ax2.set_ylabel(ylabel[1])
    ax2.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax2.xaxis.set_major_locator(mdates.AutoDateLocator())

    ax2.legend()
    ax2.grid(True)

    # Rotate x-axis labels for better readability
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45)
    plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45)

    # Set the title of the figure
    fig.suptitle(title, fontsize=16)

    # Save the current figure to the PDF
    pdf.savefig(fig)

    # Close the figure to free up memory
    plt.close(fig)

# Create a new figure for Wind
def plot_wind(plt, pdf, df, columns, title, xlabel, ylabel, colors):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6), layout="tight")

    # Plot average and gust wind speed on the first subplot
    ax1.plot(df.index, df.iloc[:, columns[1]], label=ylabel[1], color=colors[1], linewidth=2)
    ax1.plot(df.index, df.iloc[:, columns[0]], label=ylabel[0], color=colors[0], linewidth=2)
    if xlabel:
        ax1.set_xlabel(xlabel)
    ax1.set_ylabel(ylabel[0])
    #ax.tick_params(axis='y', labelcolor='b')
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax1.xaxis.set_major_locator(mdates.AutoDateLocator())
    ax1.set_xticklabels([])
    #ax1.set_xticklabels([])
    ax1.legend()
    ax1.grid(True)

    # Plot column 1 on the second subplot
    # Create a quiver plot
    #u = columns[0] * np.cos(df.iloc[:, columns[2]]/180*np.pi)
    #v = columns[0] * np.sin(df.iloc[:, columns[2]]/180*np.pi)
    #ax2.quiver(df.index, [0] * len(df.index), u, v, color=colors[2])
    hourly_avg = df.iloc[:, columns[2]].resample('H').mean()
    ax2.plot(hourly_avg.index, hourly_avg, label=ylabel[2], color=colors[2])
    if xlabel:
        ax2.set_xlabel(xlabel)
    ax2.set_ylabel(ylabel[2])
    ax2.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax2.xaxis.set_major_locator(mdates.AutoDateLocator())
    

    ax2.legend()
    ax2.grid(True)

    # Rotate x-axis labels for better readability
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45)
    plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45)

    # Set the title of the figure
    fig.suptitle(title, fontsize=16)

    # Save the current figure to the PDF
    pdf.savefig(fig)

    # Close the figure to free up memory
    plt.close(fig)

# Create a PdfPages object to save the figures
with PdfPages(pdf_file) as pdf:
    title_page(plt, pdf, tr('Weather Report') + f' {LOCATION}')

    title_page(plt, pdf, tr('Annual Overview'), 18)

    plot_data(plt, pdf, combined_df, 
              [COLUMNS['ws_temp_c'], COLUMNS['ws_humidity']], 
              tr('Air Temperature and Humidity'), None, 
              [tr('Temperature [°C]'), tr('Relative Humidity [%]')],
              [COLORS['ws_temp_c'], COLORS['ws_humidity']], 
              tr('Daily Avg.'), ['k', 'k'])

    # Calculate the difference between consecutive values in column "Rain Gauge"
    rain_df = combined_df[combined_df.iloc[:, COLUMNS['rain_mm']] != 0]
    rain_diff_df = rain_df.iloc[:, COLUMNS['rain_mm']].diff().fillna(0)
    #rain_diff_df = rain_df[rain_df >= 0]
    plot_rain(plt, pdf, rain_diff_df, rain_df, COLUMNS['rain_mm'],
              tr('Rain'), None,
              [tr('Rain [mm]'), tr('Rain [mm] (Rain Gauge)')],
              COLORS['rain_mm'])

    plot_wind(plt, pdf, combined_df,
              [COLUMNS['wind_avg'], COLUMNS['wind_gust'], COLUMNS['wind_direction']],
              tr('Wind'), None,
              [tr('Average [m/s]'), tr('Gusts [m/s]'), tr('Direction [°]')],
              [COLORS['wind_avg'], COLORS['wind_gust'], COLORS['wind_direction']])
    
    plot_data(plt, pdf, combined_df,
              [COLUMNS['battery_voltage']],
              tr('Battery Voltage'), None, [tr('Voltage [mV]')],
              [COLORS['battery_voltage']], tr('Daily Avg.'), [None])

    title_page(plt, pdf, tr('Monthly Reports'), 18)

    # Group the data by month
    combined_df['Month'] = combined_df.index.to_period('M')

    # Create a separate figure for each month
    for month, month_df in combined_df.groupby('Month'):
        title_page(plt, pdf, month, 16)

        plot_data(plt, pdf, month_df,
                  [COLUMNS['ws_temp_c'], COLUMNS['ws_humidity']],
                  tr('Air Temperature and Humidity') + f' {month}', None,
                  [tr('Temperature [°C]'), tr('Relative Humidity [%]')],
                  [COLORS['ws_temp_c'], COLORS['ws_humidity']],
                  tr('Daily Avg.'), ['k', 'k'])

        # Calculate the difference between consecutive values in column "Rain Gauge"
        
        #rain_df = month_df.iloc[:, COLUMNS['rain_mm']].diff().fillna(0)
        #rain_df = rain_df[rain_df > 0]
        rain_df = month_df[month_df.iloc[:, COLUMNS['rain_mm']] != 0]
        rain_diff_df = rain_df.iloc[:, COLUMNS['rain_mm']].diff().fillna(0)
        plot_rain(plt, pdf, rain_diff_df, rain_df, COLUMNS['rain_mm'],
                  tr('Rain') + f' {month}', None,
                  [tr('Rain [mm]'), tr('Rain [mm] (Rain Gauge)')],
                  COLORS['rain_mm'])

        plot_wind(plt, pdf, month_df,
                  [COLUMNS['wind_avg'], COLUMNS['wind_gust'], COLUMNS['wind_direction']],
                  tr('Wind') +f' {month}', None,
                  [tr('Average [m/s]'), tr('Gusts [m/s]'), tr('Direction [°]')],
                  [COLORS['wind_avg'], COLORS['wind_gust'], COLORS['wind_direction']])

        plot_data(plt, pdf, month_df,
              [COLUMNS['battery_voltage']],
              tr('Battery Voltage'), None, [tr('Voltage [mV]')],
              [COLORS['battery_voltage']], tr('Daily Avg.'), [None])
