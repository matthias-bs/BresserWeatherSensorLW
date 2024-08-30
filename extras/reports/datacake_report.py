import os
import pandas as pd
#import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.backends.backend_pdf import PdfPages

# Directory containing the CSV files
src_dir = "/home/mp/pCloudDrive/datacake/Garten"
pdf_file = "/home/mp/pCloudDrive/datacake/Garten/garten.pdf"
last_column = 16

# List all CSV files in the directory
csv_files = [f for f in os.listdir(src_dir) if f.endswith('.csv')]

# Initialize an empty list to store DataFrames
dataframes = []

# Read each CSV file
for file in csv_files:
    file_path = os.path.join(src_dir, file)
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
        if avg_colors[i]:
            daily_avg = df.iloc[:, columns[i]].resample('D').mean()
            ax.plot(daily_avg.index, daily_avg, label=f'{avg_label} {ylabels[i]}', color=avg_colors[i], linestyle='--', linewidth=2)

        ax.legend()
        ax.grid(True)
        
        # Rotate x-axis labels for better readability
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)

    fig.suptitle(title, fontsize=16)
    #plt.tight_layout()

    pdf.savefig(fig)
    plt.close(fig)

# Create a new figure for Rain Gauge
def plot_rain(plt, pdf, df_diff, df, title, xlabel, ylabel, color):
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

    # Plot column 1 on the second subplot
    ax2.plot(df.index, df.iloc[:, 6], label=ylabel[1], color=color)
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

    # Adjust layout to prevent clipping of tick-labels
    #plt.tight_layout()

    # Set the title of the figure
    fig.suptitle(title, fontsize=16)

    # Save the current figure to the PDF
    pdf.savefig(fig)

    # Close the figure to free up memory
    plt.close(fig)

# Create a new figure for Rain Gauge
def plot_wind(plt, pdf, df, columns, title, xlabel, ylabel, colors):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6))

    # Plot the Rain Gauge differences as a bar graph
    ax1.plot(df.index, df.iloc[:, columns[1]], label=ylabel[2], color=colors[1], linewidth=2)
    ax1.plot(df.index, df.iloc[:, columns[0]], label=ylabel[1], color=colors[0], linewidth=2)
    if xlabel:
        ax1.set_xlabel(xlabel)
    ax1.set_ylabel(ylabel[0])
    #ax.tick_params(axis='y', labelcolor='b')
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax1.xaxis.set_major_locator(mdates.AutoDateLocator())
    #ax1.set_xticklabels([])
    ax1.legend()
    ax1.grid(True)

    # Plot column 1 on the second subplot
    # Create a quiver plot
    #u = columns[0] * np.cos(df.iloc[:, columns[2]]/180*np.pi)
    #v = columns[0] * np.sin(df.iloc[:, columns[2]]/180*np.pi)
    #ax2.quiver(df.index, [0] * len(df.index), u, v, color=colors[2])
    ax2.plot(df.index, df.iloc[:, columns[2]], label=ylabel[2], color=colors[2])
    if xlabel:
        ax2.set_xlabel(xlabel)
    ax2.set_ylabel(ylabel[3])
    ax2.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%y %H:%M'))
    ax2.xaxis.set_major_locator(mdates.AutoDateLocator())

    ax2.legend()
    ax2.grid(True)

    # Rotate x-axis labels for better readability
    plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45)
    plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45)

    # Adjust layout to prevent clipping of tick-labels
    plt.tight_layout()

    # Set the title of the figure
    fig.suptitle(title, fontsize=16)

    # Save the current figure to the PDF
    pdf.savefig(fig)

    # Close the figure to free up memory
    plt.close(fig)

# Create a PdfPages object to save the figures
with PdfPages(pdf_file) as pdf:
    title_page(plt, pdf, 'Wetterbericht Lehmanger, Garten 95')

    title_page(plt, pdf, 'Jahresübersicht', 18)

    plot_data(plt, pdf, combined_df, [0, 1], 'Luftemperatur und -feuchte', None, ['Temperatur [°C]', 'rel. Feuchte [%]'],['r', 'b'], 'tägl. Durchschnitt', ['k', 'k'])

    # Filter out rows where soil moisture (column 9) is zero
    soil_df = combined_df[combined_df.iloc[:, 9] != 0]
    plot_data(plt, pdf, soil_df, [10, 9], 'Bodentemperatur und -feuchte', None, ['Temperatur [°C]', 'Feuchte [%]'],['m', 'g'], 'tägl. Durchschnitt', ['k', None])

    # Calculate the difference between consecutive values in column 6 (Rain Gauge)
    rain_df = combined_df.iloc[:, 6].diff().fillna(0)
    rain_df = rain_df[rain_df >= 0]
    plot_rain(plt, pdf, rain_df, combined_df, 'Niederschlag', None, ['Niederschlag [mm]', 'Niederschlag [mm] (Regenmesser)'], 'b')

    plot_data(plt, pdf, combined_df, [11], 'Warmwasser', None, ['Temperatur [°C]'], ['orange'], 'tägl. Durchschnitt', ['k'])

    plot_wind(plt, pdf, combined_df, [12, 14, 13], 'Wind', None, ['Wind [m/s]', 'Durchschnitt [m/s]', 'Böen [m/s]', 'Richtung [°]'], ['g', 'b', 'k'])

    title_page(plt, pdf, 'Monatsberichte', 18)

    # Group the data by month
    combined_df['Month'] = combined_df.index.to_period('M')

    # Create a separate figure for each month
    for month, month_df in combined_df.groupby('Month'):
        title_page(plt, pdf, month, 16)

        plot_data(plt, pdf, month_df, [0, 1], f'Luftemperatur und -feuchte {month}', None, ['Temperatur [°C]', 'rel. Feuchte [%]'],['r', 'b'], 'tägl. Durchschnitt', ['k', 'k'])

        # Filter out rows where soil moisture (column 9) is zero
        soil_df = month_df[month_df.iloc[:, 9] != 0]
        plot_data(plt, pdf, soil_df, [10, 9], f'Bodentemperatur und -feuchte {month}', None, ['Temperatur [°C]', 'Feuchte [%]'],['m', 'g'], 'tägl. Durchschnitt', ['k', None])

        # Calculate the difference between consecutive values in column 6 (Rain Gauge)
        rain_df = month_df.iloc[:, 6].diff().fillna(0)
        rain_df = rain_df[rain_df >= 0]
        plot_rain(plt, pdf, rain_df, month_df, f'Niederschlag {month}', None, ['Niederschlag [mm]', 'Niederschlag [mm] (Regenmesser)'], 'b')

        plot_data(plt, pdf, month_df, [11], f'Warmwasser {month}', None, ['Temperatur [°C]'], ['orange'], 'tägl. Durchschnitt', ['k'])

        plot_wind(plt, pdf, month_df, [12, 14, 13], f'Wind {month}', None, ['Wind [m/s]', 'Durchschnitt [m/s]', 'Böen [m/s]', 'Richtung [°]'], ['g', 'b', 'k'])

