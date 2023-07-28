# PSoC 6 MCU: Cyberon DSpotter Demo

This demo package demonstrates how to use Cyberon DSpotter Lib for CY8CKIT-062S2-43012 kit.

## Requirements

- [ModusToolbox&trade; software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) v3.0 or later (tested with v3.0)
- PSoC&trade; 6 Board support package (BSP) minimum required version: 4.0.0
- Programming language: C
- Associated parts: All [PSoC&trade; 6 MCU](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu) parts

## Supported Toolchains (make variable 'TOOLCHAIN')

- GNU Arm® embedded compiler v10.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`

## Supported Kits (make variable 'TARGET')

- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062s2-43012/) (`CY8CKIT-062S2-43012`)

## Hardware Setup

This demo package uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

## Software Setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

## Obtaining License File

Go to [Cyberon DSpotter License for Infineon Platform](https://license.cyberon.tw/InfineonDSpotterLicense/InfineonDSpotterLicense.php) to obtain a license file for the Cyberon keyword detection engine. The chipset ID can be accessed by executing the demo package.

**Note:** Each chipset ID has a limit of obtaining license file 10 times at most.

## Using the Demo Package

### In Eclipse IDE for ModusToolbox:

1. Click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox Application**). This launches the [Project Creator](http://www.cypress.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project Creator - Choose Board Support Package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, click the link from the Quick Panel. 

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project Creator - Select Application** dialog, choose the example by enabling the checkbox.

4. Optionally, change the suggested **New Application Name**.

5. Enter the local path in the **Application(s) Root Path** field to indicate where the application needs to be created. 

   Applications that can share libraries can be placed in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox User Guide](https://www.cypress.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

### In Command-line Interface (CLI):

ModusToolbox provides the Project Creator as both a GUI tool and a command line tool to easily create one or more ModusToolbox applications. See the "Project Creator Tools" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) for more details.

Alternatively, you can manually create the application using the following steps:

1. Download and unzip this repository onto your local machine, or clone the repository.

2. Open a CLI terminal and navigate to the application folder.

   On Linux and macOS, you can use any terminal application. On Windows, open the **modus-shell** app from the Start menu.

   **Note:** The cloned application contains a default BSP file (*TARGET_xxx.mtb*) in the *deps* folder. Use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) (`make modlibs` command) to select and download a different BSP file, if required. If the selected kit does not have the required resources or is not [supported](#supported-kits-make-variable-target), the application may not work. 

3. Import the required libraries by executing the `make getlibs` command.

Various CLI tools include a `-h` option that prints help information to the terminal screen about that tool. For more details, see the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*).

### In Third-party IDEs:

1. Follow the instructions from the [CLI](#in-command-line-interface-cli) section to create the application, and import the libraries using the `make getlibs` command.

2. Export the application to a supported IDE using the `make <ide>` command. 

    For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*.

3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Replace **CybLicense.bin** in the **data** directory with your license file using the same file name.

3. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

4. Program the board.

   - **Using Eclipse IDE for ModusToolbox:**

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.

   - **Using CLI:**

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. You can specify a target and toolchain manually:
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=CY8CKIT-062S2-43012 TOOLCHAIN=GCC_ARM
      ```

   ![serial-terminal](./images/serial-terminal.png)

There are two types of recognition:

| Types      | Trigger word       | Command                 |
|----------  |--------------------|-------------------------|
| One-stage  | Hello CyberVoice   |                         |
| Two-stage  | Hello CyberVoice   | Open camera             |
|            |                    | Take a picture          |
|            |                    | Play music              |
|            |                    | Stop music              |
|            |                    | Volume louder           |
|            |                    | Volume softer           |

- One-stage recognition with only trigger word **Hello CyberVoice**.
- Two-stage recognition with trigger word **Hello CyberVoice** and six commands (**Open camera, Take a picture, Play music, Stop music, Volume louder, Volume softer**). When the demo program is launched, the recognition is at [trigger stage], and only the trigger words will be detected. Once a trigger word is detected, the recognition system enters [command stage] and performs the recognition of command words. If no command is recognized within 10 seconds (changable), it will back to [trigger stage].
- For this demo, it only supports **One-stage** recognition.
- After programming, the application starts automatically. Confirm that the serial terminal reports the status.
- Say trigger word or commands over the microphone. If the keyword is detected, the result will be shown on the serial terminal, along with the command ID as well as other information, such as confidence scores and volume energy.
- This demo version has a limit of 50 times recognitions.

## Create Custom Model

1. Go to [Cyberon DSMT Tool V2 Help](https://tool.cyberon.com.tw/DSMT_V2/index.php?lang=en) to learn DSpotter Modeling Tool(DSMT) interface, and download the application here.
2. Log in DSMT with trial account below:
   - Account: infineon_trial@cyberon.com.tw
   - Password: 
   - Keep password field blank.
3. Note that the DSMT account will work only on the Windows PC.
4. Create a project, add trigger word into Group 1, and add commands into Group 2 for two-stage recognition. After confirming all keywords, please go to the **Extra Output** box at the right bottom, check the **Command Text** to pack the model with files of command text, then click **Save Project**. The model bin file named **[project_name]_pack_withTxt.bin** will be generated under this project path.

Find more details about DSMT in the following tutorial materials:
- [DSMT Tutorial Slide](https://drive.google.com/file/d/1kWrJ7OXgHTF4YPft6WRHeVlXVCGiqTNx/view)
- [DSMT Tutorial Video](https://www.youtube.com/playlist?list=PLTEknqO5GAbrDX5NMs-P6b9THWwamgVBo)


## Import Custom Model

Replace **Trigger_pack_withTxt.bin** in **data** directory with your custom model bin file using the same file name.

