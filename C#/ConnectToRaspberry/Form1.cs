using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Management;
using NativeWifi;

namespace ConnectToRaspberry
{
    public partial class Form1 : Form
    {
        private const string NETMASK = "255.255.255.0";
        private Process process;
        public Form1()
        {
            //process = new Process();
            //process.StartInfo.UseShellExecute = false;
            //process.StartInfo.CreateNoWindow = true;
            //process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;

            //InitializeComponent();

            //connection_button.BackColor = Color.Red;
            //disconnection_button.BackColor = Color.Red;
            //IPList.Items.Add("192.168.1.200");
            //IPList.Items.Add("192.168.1.201");
            //IPList.Items.Add("192.168.1.202");
            //IPList.Items.Add("192.168.1.203");
            //IPList.Items.Add("192.168.1.204");
            //IPList.Items.Add("192.168.1.205");
            //IPList.Items.Add("192.168.1.206");
            //IPList.Items.Add("192.168.1.207");
            //IPList.Items.Add("192.168.1.208");
            //IPList.Items.Add("192.168.1.209");
            //IPList.SelectedIndex = 0;

            //disconnectFromPeer();
            //setDHCPAddress();
            //disconnection_button.BackColor = Color.Green;

            getInfoWifi();
        }

        public bool isUserAdmin()
        {
            bool isAdmin;
            try
            {
                WindowsIdentity user = WindowsIdentity.GetCurrent();
                WindowsPrincipal principal = new WindowsPrincipal(user);
                isAdmin = principal.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch (UnauthorizedAccessException)
            {
                isAdmin = false;
            }
            catch (Exception)
            {
                isAdmin = false;
            }

            return isAdmin;
        }

        /// <summary> 
        /// Set's a new IP Address and it's Submask of the local machine 
        /// </summary> 
        /// <param name="ip_address">The IP Address</param> 
        /// <param name="subnet_mask">The Submask IP Address</param> 
        /// <remarks>Requires a reference to the System.Management namespace</remarks> 
        private void setIP(string address, string subnetMask, string gateway)
        {
            process.StartInfo.FileName = "netsh";
            process.StartInfo.Arguments = "interface ip set address \"Wi-Fi\" static " + address + " " + subnetMask + " " + gateway;
            try
            {
                using (Process execute = Process.Start(process.StartInfo))
                {
                    execute.WaitForExit();
                }
            }
            catch { }
        }

        private void setDHCPAddress()
        {
            process.StartInfo.FileName = "netsh";
            process.StartInfo.Arguments = "interface ip set address \"Wi-Fi\" dhcp";
            try
            {
                using (Process execute = Process.Start(process.StartInfo))
                {
                    execute.WaitForExit();
                }
            }
            catch { }
        }

        private void connectToPeer(String peerNodeName)
        {
            process.StartInfo.FileName = "netsh";
            process.StartInfo.Arguments = "wlan connect " + peerNodeName;
            try
            {
                using (Process execute = Process.Start(process.StartInfo))
                {
                    execute.WaitForExit();
                }
            }
            catch { }
        }

        private void disconnectFromPeer()
        {
            process.StartInfo.FileName = "netsh";
            process.StartInfo.Arguments = "wlan disconnect";
            try
            {
                using (Process execute = Process.Start(process.StartInfo))
                {
                    execute.WaitForExit();
                }
            }
            catch { }
        }

        private void connection_button_Click(object sender, EventArgs e)
        {
            setIP(IPList.GetItemText(IPList.SelectedItem), NETMASK, "");
            connectToPeer("MicroITS");
            connection_button.BackColor = Color.Green;
            disconnection_button.BackColor = Color.Red;
        }

        private void disconnection_button_Click(object sender, EventArgs e)
        {
            setDHCPAddress();
            disconnectFromPeer();
            connection_button.BackColor = Color.Red;
            disconnection_button.BackColor = Color.Green;
        }

        private void getInfoWifi()
        {
            WlanClient client = new WlanClient();
            foreach (WlanClient.WlanInterface wlanIface in client.Interfaces)
            {
                // Lists all networks with WEP security
                Console.WriteLine(wlanIface.NetworkInterface);
                Wlan.WlanAvailableNetwork[] networks = wlanIface.GetAvailableNetworkList(0);
                foreach (Wlan.WlanAvailableNetwork network in networks)
                {
                    if (network.dot11DefaultCipherAlgorithm == Wlan.Dot11CipherAlgorithm.WPA_UseGroup)
                    {
                        Console.WriteLine("Found WEP network with SSID {0}.", GetStringForSSID(network.dot11Ssid));
                    }
                    Console.WriteLine("Found WEP network with SSID {0}.", GetStringForSSID(network.dot11Ssid));
                }

                // Retrieves XML configurations of existing profiles.
                // This can assist you in constructing your own XML configuration
                // (that is, it will give you an example to follow).
                //foreach (Wlan.WlanProfileInfo profileInfo in wlanIface.GetProfiles())
                //{
                //    string name = profileInfo.profileName; // this is typically the network's SSID
                //    string xml = wlanIface.GetProfileXml(profileInfo.profileName);
                //}
            }
        }

        static string GetStringForSSID(Wlan.Dot11Ssid ssid)
        {
            return Encoding.ASCII.GetString(ssid.SSID, 0, (int)ssid.SSIDLength);
        }
    }
}
