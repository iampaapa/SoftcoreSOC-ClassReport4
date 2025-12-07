# Class Report 4: Inclination Sensing

This repo contains the source code and instructions to replicate the **Inclination Sensing Experiment** (Exp 16.8.1).

**Demo Video:** [https://youtube.com/shorts/oy8GMESb3IY](https://youtube.com/shorts/oy8GMESb3IY)

## Directory Structure

  * **`hw/`**: Contains SystemVerilog source code (`hdl/`) and constraints (`constraints/`) for the FPGA.
  * **`sw/`**: Contains the C++ drivers (`drivers/`) and the main application code (`app/`) for the MicroBlaze processor.

-----

## Build Instructions

### 1: Hardware Build (Vivado)

1.  Open **Vivado** and create a new **RTL Project**.
2.  **Add Sources:**
      * Add the entire `hw/hdl` directory.
      * Add the constraint file `hw/constraints/Nexys4_DDR_chu.xdc`.
3.  **Configure MicroBlaze MCS IP:**
      * Open the **IP Catalog** and search for "MicroBlaze MCS".
      * Double-click to customize:
          * **Component Name:** `cpu`.
          * **Memory Size:** `128KB`.
          * **Input Clock:** `100` MHz.
          * **Interface:** Check **Enable I/O Bus**.
      * Click **Generate**.
4.  **Set Top Module:**
      * Find and set `sys/top/mcs_top_sampler.sv` as Top.
5.  **Generate Hardware:**
      * Click **Generate Bitstream**.
      * Once complete, go to **File -\> Export -\> Export Hardware**.
      * Check **Include Bitstream** and save the `.xsa` file.

### 2: Software Build (Vitis)

1.  Open **Vitis** and select a workspace.
2.  **Create Platform:**
      * **File -\> New -\> Platform Project**.
      * Create a platform from the `.xsa` file exported in 1.
      * Build the platform project.
3.  **Create Application:**
      * **File -\> New -\> Application Project**.
      * Select the platform created above.
4.  **Import Source Code:**
      * Right-click the `src` folder in your application project -\> **Import** -\> **File System**.
      * Import all files from this repo's `sw/drivers/` folder.
      * Import `sw/app/inclination_sensing.cpp`.
5.  **Build Application:**
      * Build the project (Hammer icon).

### 3: Final Integration (Vivado)

To prevent memory mapping errors (which was happening when I was using Vitis directly), I embedded the software directly into the FPGA bitstream.

1.  Come back to **Vivado**.
2.  **Add the ELF:**
      * **Add Sources** -\> **Add or create design sources**.
      * Navigate to your Vitis workspace and select the generated `.elf` file (e.g., `inclination_app.elf`).
3.  **Associate ELF:**
      * Go to **Tools -\> Associate ELF Files**.
      * Find the processor instance (`system_i/cpu`).
      * Click the file field under **Design Sources** and select your `.elf` file.
      * Ensure it is applied to the **Implementation** state.
4.  **Re-Generate Bitstream:**
      * Click **Generate Bitstream** again. Vivado will now merge the C++ program into the Block RAM initialization vectors.
5.  **Program Device:**
      * Open **Hardware Manager**, connect to the board, and program with the final `.bit` file.

-----

## Running the Project

1.  Connect the Nexys 4 DDR board via USB and ensure it is powered ON.
2.  **Tilt the board:**
      * **Tilt Left:** LED[3] turns ON.
      * **Tilt Right:** LED[1] turns ON.
      * **Inverted:** LED[2] turns ON.
      * **Upright:** LED[0] turns ON.
      * **Flat:** All 4 LEDs turn ON.

-----

### **Running the Project using the Pre-built Binaries**

To actually run the project without rebuilding the entire hardware and software stack, you can use the pre-compiled artifacts located in the `bin/` directory.

**Files Included:**
* `mcs_top_sampler.bit`: The complete FPGA bitstream with the C++ application already embedded.
* `project_inclination.xsa`: The hardware specification file (for Vitis).
* `inclination_app.elf`: The compiled software executable.

**Steps to use:**
1.  Connect your **Nexys 4 DDR** board via USB and turn it ON.
2.  Open **Vivado**.
3.  Click **Open Hardware Manager** under **Tasks**
4.  Click **Open Target** -> **Auto Connect**.
5.  Click **Program Device**.
6.  Browse to and select `bin/mcs_top_sampler.bit`.
7.  Click **Program**.

**Expected Results:**
* LED[3] turns ON when the board is tilted left
* LED[1] turns ON when the board is tilted right
* LED[2] turns ON when the board is inverted
* LED[0] turns ON when the board is upright
* All 4 LEDs turn ON when the board is flat