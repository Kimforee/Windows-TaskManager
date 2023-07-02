# Windows-TaskManager using QtCreator
1) First page with process monitering 2) CPU usage live graph
![1,2](https://github.com/Kimforee/Windows-TaskManager/assets/97099667/48e322db-dc53-4fd5-9e06-e1fc3d3898f5)
3) RAM usage live Graph 4) Process Boosting (yellow coloured process)
![3,4](https://github.com/Kimforee/Windows-TaskManager/assets/97099667/e8865354-1d2a-4f04-a016-bba1ee8bb2ac)
5) Process Unboost (red coloured process) 6) Process Termination
![5,6](https://github.com/Kimforee/Windows-TaskManager/assets/97099667/cc79c24a-ee24-4918-8649-a0d051faa95e)

Task Manager is a system monitoring application built with Qt that allows you to view and
manage running processes and monitor resource utilization in real-time.

#-Features-
.View a list of currently running processes.
.Monitor CPU utilization.
.Monitor memory (RAM) utilization.
.Monitor disk utilization.
.Monitor network utilization.
.Terminate processes.
.Boost/unboost process priority.
.Search and filter processes.(yet to be added)
.Sort processes by various attributes (e.g., CPU usage, memory usage)(stop refreshing first).
.Graphical representation of resource utilization.

#-Requirements-
Qt (version 6.5.1 or higher)
Additional libraries or dependencies (QChart,Pdh)

#-Installation-
Clone the repository or download the source code.
Open the project in Qt Creator.
Configure any necessary project settings or dependencies.
Build the project.
Run the executable.

#Usage
Launch the Task Manager application.
The main window displays the list of running processes, along with their relevant details
such as process ID,process name, CPU usage, memory usage, and status etc
To terminate a process, select it from the list and click the "Terminate" button.
To boost or unboost the priority of a process, select it from the list and click the "Boost" or "Unboost" button.
Use the search bar to search for a specific process by name or process ID.
Click on the column headers to sort the processes based on various attributes.
The resource utilization graphs provide a visual representation of CPU, memory, disk, and network usage. The graphs are updated in real-time.
Customize the settings or options as per your requirements.

Contributing
Contributions are welcome! If you would like to contribute to the Task Manager project, please follow these steps:

Fork the repository.
Create a new branch for your feature or bug fix.
Make your changes and commit them.
Push your changes to your forked repository.
Submit a pull request with a detailed description of your changes.

Acknowledgements
https://stackoverflow.com (cpu usage, memory usage)
https://learn.microsoft.com (about windows api)
https://chat.openai.com and https://doc.qt.io/qt-6/widget-classes.html#graphics-view-classes (qt widgets)
