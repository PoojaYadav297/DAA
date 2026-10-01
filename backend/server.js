const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();

const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());


// ==========================================
// HOME
// ==========================================

app.get("/", (req, res) => {
    res.sendFile(
        path.join(__dirname, "..", "index.html")
    );
});


// ==========================================
// TEST API
// ==========================================

app.get("/api/test", (req, res) => {

    res.json({
        message: "Smart Travel Planner backend is working!"
    });

});


// ==========================================
// TRAVEL PLANNER API
// ==========================================

app.post("/api/plan-trip", async (req, res) => {

    try {

        const {
            startLocation,
            places,
            finalLocation,
            availableTime
        } = req.body;


        // ------------------------------------------
        // Validate input
        // ------------------------------------------

        if (!startLocation) {
            return res.status(400).json({
                error: "Starting location is required."
            });
        }

        if (!finalLocation) {
            return res.status(400).json({
                error: "Final location is required."
            });
        }

        if (!places || !Array.isArray(places)) {
            return res.status(400).json({
                error: "Places are required."
            });
        }

        if (places.length === 0) {
            return res.status(400).json({
                error: "At least one place is required."
            });
        }

        if (!availableTime || availableTime <= 0) {
            return res.status(400).json({
                error: "Available time must be greater than 0."
            });
        }


        // ------------------------------------------
        // Display received data
        // ------------------------------------------

        console.log("\n========== NEW TRIP ==========");

        console.log("Start:", startLocation);

        console.log("Places:");

        places.forEach((place, index) => {

            console.log(
                `${index + 1}. ${place.name} - ${place.exploreTime} hours`
            );

        });

        console.log("Final:", finalLocation);

        console.log("Available Time:", availableTime);

        console.log("==============================\n");


        // ------------------------------------------
        // TODO:
        // Call GPS / Routing API here
        // ------------------------------------------

        /*
            The routing API will provide something like:

            Start -> Place 1
            Start -> Place 2
            Start -> Place 3

            Place 1 -> Place 2
            Place 1 -> Place 3

            Place 2 -> Place 1
            Place 2 -> Place 3

            ...

            Place N -> Final
        */


        // Temporary response
        // We will replace this after connecting
        // the GPS API.

        const result = {

            message: "Input received successfully.",

            startLocation: startLocation,

            places: places,

            finalLocation: finalLocation,

            availableTime: availableTime,

            status: "GPS API connection pending"

        };


        res.json(result);


    } catch (error) {

        console.error(error);

        res.status(500).json({
            error: "Server error while planning trip."
        });

    }

});


// ==========================================
// START SERVER
// ==========================================

app.listen(PORT, () => {

    console.log(
        `Smart Travel Planner backend running at http://localhost:${PORT}`
    );

});
