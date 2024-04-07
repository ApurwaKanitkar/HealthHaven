import {initializeApp} from 'firebase/app'
import {getFirestore, collection, doc, getDoc, onSnapshot} from 'firebase/firestore'

// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
    apiKey: "AIzaSyD_6GeDPu1MiHL85Ts7lvCyHvkF5N6PgsE",
    authDomain: "health-haven-4c78a.firebaseapp.com",
    databaseURL: "https://health-haven-4c78a-default-rtdb.firebaseio.com",
    projectId: "health-haven-4c78a",
    storageBucket: "health-haven-4c78a.appspot.com",
    messagingSenderId: "88986625501",
    appId: "1:88986625501:web:223fb9f9a2781952328234",
    measurementId: "G-2HKPRRQ37S"
  };

// init firebase app
initializeApp(firebaseConfig)

// init services
const db = getFirestore()

// collection ref
const colRef = collection(db, 'sensordata')

// Document reference (within the collection)
const docRef = doc(colRef, "patient")

// List to store retrieved data
const records = []

// Initialize separate lists for heart rate, spo2, and temperature
const heartRates = [];
const spo2Values = [];
const temperatures = [];

// get document data
getDoc(docRef)
  .then((docSnapshot) => {
    // Get the document data
    const data = docSnapshot.data();

    records.push(...data.appended_data);
    console.log("Records retrieved successfully:",records);
  });

// Function to parse each entry in records list and extract values
function parseRecord(record) {
  // Remove brackets and split the string by commas
  const values = record.slice(1, -2).split(',');
  // Parse each value to integer
  const [heartRate, spo2, temperature] = values.map(value => parseInt(value));
  // Append values to respective lists
  heartRates.push(heartRate);
  spo2Values.push(spo2);
  temperatures.push(temperature);
}

// Parse existing records
records.forEach(record => parseRecord(record));

// Function to handle real-time updates
function handleRealTimeUpdate(docSnapshot) {
    const newData = docSnapshot.data().appended_data;
    // Parse new data and append to respective lists
    newData.forEach(record => parseRecord(record));
    console.log("Records updated successfully:");
    console.log("Heart Rates:", heartRates);
    console.log("SpO2 Values:", spo2Values);
    console.log("Temperatures:", temperatures);
}

// Get document data and listen for real-time updates
getDoc(docRef)
    .then(docSnapshot => {
        handleRealTimeUpdate(docSnapshot);
        // Listen for real-time updates
        docRef.onSnapshot(handleRealTimeUpdate);
    })
    .catch(error => {
        console.error("Error getting document:", error);
    });

const times = ['7-9 am','9-11 am','11-1 pm','1-3 pm','3-5 pm','5-7 pm','7-9 pm','9-11 pm','11-1 am','1-3 am', '2-5 am','5-7 am'];
const heartRateData = heartRates; // BPM
const spo2Data = spo2Values; // SpO2 percentage
const tempData = temperatures; // Body temperature (Fahrenheit)

// Create individual line charts
const createChart = (id, label, data) => {
    const ctx = document.getElementById(id).getContext('2d');
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: times,
            datasets: [
                {
                    label,
                    data,
                    borderColor: 'blue',
                    fill: false,
                },
            ],
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Time of the day',
                    },
                },
                y: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Values',
                    },
                },
            },
        },
    });
};

createChart('heartRateChart', 'Heart Rate (BPM)', heartRateData);
createChart('spo2Chart', 'SpO2 (%)', spo2Data);
createChart('tempChart', 'Body Temperature (°F)', tempData);