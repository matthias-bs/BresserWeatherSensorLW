import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.backends.backend_pdf import PdfPages

# Directory containing the CSV files
dir = "/home/mp/pCloudDrive/datacake/Garten"
last_column = 16

# List all CSV files in the directory
csv_files = [f for f in os.listdir(dir) if f.endswith('.csv')]

# Initialize an empty list to store DataFrames
dataframes = []

# Read each CSV file
for file in csv_files:
    file_path = os.path.join(dir, file)
    df = pd.read_csv(file_path, skiprows=0, parse_dates=[0], date_parser=lambda x: pd.to_datetime(x, format='%a, %d %b %Y %H:%M:%S'))
    
    for col in range(1, last_column):
        # Replace comma with dot in the second column and convert to float
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
combined_df = combined_df[combined_df.iloc[:, 1] != 0]

# Create a PdfPages object to save the figures
with PdfPages('garten.pdf') as pdf:
    # Create a title page
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.text(0.5, 0.5, 'Wetterbericht Lehmanger, Garten 95', transform=ax.transAxes, fontsize=24, ha='center', va='center')
    ax.axis('off')
    pdf.savefig(fig)
    plt.close(fig)

    # Create a figure with two subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 12))

    # Plot column 0 on the first subplot
    ax1.plot(combined_df.index, combined_df.iloc[:, 0], label='Temperatur [°C]', color='r')
    ax1.set_xlabel('Zeit')
    ax1.set_ylabel('Temperatur [°C]', color='r')
    ax1.tick_params(axis='y', labelcolor='r')
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax1.xaxis.set_major_locator(mdates.AutoDateLocator())
    
    # Calculate and plot daily average for temperature
    daily_avg_temperature = combined_df.iloc[:, 0].resample('D').mean()
    ax1.plot(daily_avg_temperature.index, daily_avg_temperature, label='Täglicher Durchschnitt [°C]', color='k', linestyle='--', linewidth=2)

    ax1.legend()
    ax1.grid(True)

    # Plot column 1 on the second subplot
    ax2.plot(combined_df.index, combined_df.iloc[:, 1], label='rel. Feuchte [%]', color='b')
    ax2.set_xlabel('Zeit')
    ax2.set_ylabel('rel. Feuchte [%]', color='b')
    ax2.tick_params(axis='y', labelcolor='b')
    ax2.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax2.xaxis.set_major_locator(mdates.AutoDateLocator())

    # Calculate and plot daily average for humidity
    daily_avg_humidity = combined_df.iloc[:, 1].resample('D').mean()
    ax2.plot(daily_avg_humidity.index, daily_avg_humidity, label='Täglicher Durchschnitt [%]', color='k', linestyle='--', linewidth=2)

    ax2.legend()
    ax2.grid(True)

    # Rotate x-axis labels for better readability
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45)
    plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45)

    # Adjust layout to prevent clipping of tick-labels
    plt.tight_layout()

    # Show the plot
    #plt.show()

    # Save the current figure to the PDF
    pdf.savefig(fig)
    
    # Close the figure to free up memory
    plt.close(fig)

    # Group the data by month
    combined_df['Month'] = combined_df.index.to_period('M')

    # Create a separate figure for each month
    for month, month_df in combined_df.groupby('Month'):
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 12))

        # Plot column 0 on the first subplot
        #ax1.scatter(month_df.index, month_df.iloc[:, 0], label='Temperatur [°C]', color='r', s=10, marker='+')
        ax1.plot(month_df.index, month_df.iloc[:, 0], label='Temperatur [°C]', color='r')
        ax1.set_xlabel('Zeit')
        ax1.set_ylabel('Temperatur [°C]', color='r')
        ax1.tick_params(axis='y', labelcolor='r')
        ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax1.xaxis.set_major_locator(mdates.AutoDateLocator())

        # Calculate and plot daily average for temperature
        daily_avg_temperature = month_df.iloc[:, 0].resample('D').mean()
        ax1.plot(daily_avg_temperature.index, daily_avg_temperature, label='Täglicher Durchschnitt [°C]', color='r', linestyle='--', linewidth=2)

        ax1.legend()
        ax1.grid(True)

        # Plot column 1 on the second subplot
        #ax2.scatter(month_df.index, month_df.iloc[:, 1], label='rel. Feuchte [%]', color='b', s=10, marker='+')
        ax2.plot(month_df.index, month_df.iloc[:, 1], label='rel. Feuchte [%]', color='b')
        ax2.set_xlabel('Zeit')
        ax2.set_ylabel('rel. Feuchte [%]', color='b')
        ax2.tick_params(axis='y', labelcolor='b')
        ax2.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax2.xaxis.set_major_locator(mdates.AutoDateLocator())
        

        # Calculate and plot daily average for humidity
        daily_avg_humidity = month_df.iloc[:, 1].resample('D').mean()
        ax2.plot(daily_avg_humidity.index, daily_avg_humidity, label='Täglicher Durchschnitt [%]', color='b', linestyle='--', linewidth=2)

        ax2.legend()
        ax2.grid(True)

        # Rotate x-axis labels for better readability
        plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45)
        plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45)

        # Adjust layout to prevent clipping of tick-labels
        plt.tight_layout()

        # Set the title of the figure
        fig.suptitle(f'Garten {month} - Luft', fontsize=16)

        # Show the plot
        #plt.show()

        # Save the current figure to the PDF
        pdf.savefig(fig)
        
        # Close the figure to free up memory
        plt.close(fig)

        # Filter out rows where soil moisture (column 9) is zero
        month_soil_df = month_df[month_df.iloc[:, 9] != 0]

        # Create a new figure for Soil Temperature and Soil Moisture
        fig, (ax3, ax4) = plt.subplots(2, 1, figsize=(12, 12))
        
        # Plot column 10 (Soil Temperature) on the first subplot as small dots
        ax3.plot(month_soil_df.index, month_soil_df.iloc[:, 10], label='Bodentemperatur [°C]', color='g')
        ax3.set_xlabel('Zeit')
        ax3.set_ylabel('Bodentemperatur [°C]', color='g')
        ax3.tick_params(axis='y', labelcolor='g')
        ax3.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax3.xaxis.set_major_locator(mdates.AutoDateLocator())
        
        # Calculate and plot daily average for Soil Temperature
        daily_avg_soil_temp = month_soil_df.iloc[:, 10].resample('D').mean()
        avg_soil_temp = daily_avg_soil_temp.mean()
        ax3.plot(daily_avg_soil_temp.index, daily_avg_soil_temp, label='Täglicher Durchschnitt [°C]', color='g', linestyle='--', linewidth=2)
        
        ax3.legend()
        ax3.grid(True)

        # Plot column 9 (Soil Moisture) on the second subplot as small dots
        ax4.plot(month_soil_df.index, month_soil_df.iloc[:, 9], label='Bodenfeuchte [%]', color='m')
        ax4.set_xlabel('Zeit')
        ax4.set_ylabel('Bodenfeuchte [%]', color='m')
        ax4.tick_params(axis='y', labelcolor='m')
        ax4.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax4.xaxis.set_major_locator(mdates.AutoDateLocator())
        
        # Calculate and plot daily average for Soil Moisture
        daily_avg_soil_moisture = month_soil_df.iloc[:, 9].resample('D').mean()
        avg_soil_moisture = daily_avg_soil_moisture.mean()
        ax4.plot(daily_avg_soil_moisture.index, daily_avg_soil_moisture, label='Täglicher Durchschnitt [%]', color='m', linestyle='--', linewidth=2)
        
        ax4.legend()
        ax4.grid(True)

        # Rotate x-axis labels for better readability
        plt.setp(ax3.xaxis.get_majorticklabels(), rotation=45)
        plt.setp(ax4.xaxis.get_majorticklabels(), rotation=45)
        
        # Adjust layout to prevent clipping of tick-labels
        plt.tight_layout()

        # Set the title of the figure
        fig.suptitle(f'Garten {month} - Boden', fontsize=16)
        
        # Save the current figure to the PDF
        pdf.savefig(fig)
        
        # Close the figure to free up memory
        plt.close(fig)

        # Create a new figure for Water Temperature
        fig, ax5 = plt.subplots(1, 1, figsize=(12, 12))

        # Plot column 11 (Water Temperature)
        ax5.plot(month_df.index, month_df.iloc[:, 11], label='Wassertemperatur [°C]', color='orange')
        ax5.set_xlabel('Zeit')
        ax5.set_ylabel('Wassertemperatur [°C]', color='orange')
        ax5.tick_params(axis='y', labelcolor='orange')
        ax5.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax5.xaxis.set_major_locator(mdates.AutoDateLocator())
        
        # Calculate and plot daily average for Water Temperature
        daily_avg_water_temp = month_df.iloc[:, 11].resample('D').mean()
        ax5.plot(daily_avg_water_temp.index, daily_avg_water_temp, label='Täglicher Durchschnitt [°C]', color='orange', linestyle='--', linewidth=2)
        
        ax5.legend()
        ax5.grid(True)

        # Rotate x-axis labels for better readability
        plt.setp(ax5.xaxis.get_majorticklabels(), rotation=45)

        
        # Adjust layout to prevent clipping of tick-labels
        plt.tight_layout()

        # Set the title of the figure
        fig.suptitle(f'Garten {month} - Warmwasser', fontsize=16)
        
        # Save the current figure to the PDF
        pdf.savefig(fig)
        
        # Close the figure to free up memory
        plt.close(fig)

        # Create a new figure for Rain Gauge
        fig, ax6 = plt.subplots(1, 1, figsize=(12, 6))

        # Calculate the difference between consecutive values in column 6 (Rain Gauge)
        rain_diff = month_df.iloc[:, 6].diff().fillna(0)

        # Plot the Rain Gauge differences as a bar graph
        ax6.bar(month_df.index, rain_diff, label='Niederschlag [mm]', color='b')
        ax6.set_xlabel('Zeit')
        ax6.set_ylabel('Niederschlag [mm]', color='b')
        ax6.tick_params(axis='y', labelcolor='b')
        ax6.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
        ax6.xaxis.set_major_locator(mdates.AutoDateLocator())

        #ax6.plot(month_df.index, month_df.iloc[:, 6], label='Täglicher Durchschnitt [°C]', color='b', linestyle='--', linewidth=2)

        ax6.legend()
        ax6.grid(True)

        # Rotate x-axis labels for better readability
        plt.setp(ax6.xaxis.get_majorticklabels(), rotation=45)

        # Adjust layout to prevent clipping of tick-labels
        plt.tight_layout()

        # Set the title of the figure
        fig.suptitle(f'Garten {month} - Regenmesser', fontsize=16)

        # Save the current figure to the PDF
        pdf.savefig(fig)

        # Close the figure to free up memory
        plt.close(fig)
