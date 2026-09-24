using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

public class UdpReceiver : MonoBehaviour
{
    UdpClient udpClient;
    Thread receiveThread;

    public int port = 5005; // Должен совпадать с Python
    public Transform target; // Сюда передаём ориентацию

    private Vector3 latestEulerAngles;

    void Start()
    {
        udpClient = new UdpClient(port);
        receiveThread = new Thread(new ThreadStart(ReceiveData));
        receiveThread.IsBackground = true;
        receiveThread.Start();
        Debug.Log("🟢 UDP-Receiver gestartet auf Port " + port);
    }

    void ReceiveData()
    {
        while (true)
        {
            try
            {
                IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, port);
                byte[] data = udpClient.Receive(ref anyIP);
                string text = Encoding.UTF8.GetString(data);
                // Пример строки: "12.3,45.6,78.9"
                string[] parts = text.Trim().Split(',');

                if (parts.Length == 3 &&
                    float.TryParse(parts[0], out float x) &&
                    float.TryParse(parts[1], out float y) &&
                    float.TryParse(parts[2], out float z))
                {
                    latestEulerAngles = new Vector3(x, y, z);
                }
            }
            catch (System.Exception e)
            {
                Debug.LogWarning("❌ UDP Fehler: " + e.Message);
            }
        }
    }

    void Update()
    {
        // Обновляем объект в главном потоке Unity
        if (target != null)
        {
           target.rotation = Quaternion.Euler(latestEulerAngles.z, latestEulerAngles.x, latestEulerAngles.y);

        }
    }

    void OnApplicationQuit()
    {
        receiveThread?.Abort();
        udpClient?.Close();
    }
}
