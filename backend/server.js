const express = require("express");
const cors = require("cors");

const app = express();

const PORT = 3000;


// ==========================================
// MIDDLEWARE
// ==========================================

app.use(cors());

app.use(express.json());


// ==========================================
// TEST ROUTE
// ==========================================

app.get("/", (req, res) => {

    res.json({
        message: "Smart Travel Planner backend is running."
    });

});


// ==========================================
// TEST API
// ==========================================

app.get("/api/test", (req, res) => {

    res.json({
        success: true,
        message: "Backend connection is working."
    });

});


// ==========================================
// PLAN TRIP
// ==========================================

app.post("/api/plan-trip", (req, res) => {

    try {

        const {
            startLocation,
            places,
            finalLocation,
            availableTime
        } = req.body;


        // --------------------------------------
        // VALIDATION
        // --------------------------------------

        if (!startLocation) {

            return res.status(400).json({
                success: false,
                error: "Starting location is required."
            });

        }


        if (!places || !Array.isArray(places)) {

            return res.status(400).json({
                success: false,
                error: "Places are required."
            });

        }


        if (places.length === 0) {

            return res.status(400).json({
                success: false,
                error: "Please enter at least one place."
            });

        }


        if (!finalLocation) {

            return res.status(400).json({
                success: false,
                error: "Final location is required."
            });

        }


        if (
            !availableTime ||
            availableTime <= 0
        ) {

            return res.status(400).json({
                success: false,
                error: "Available time must be greater than 0."
            });

        }


        // --------------------------------------
        // CHECK EACH PLACE
        // --------------------------------------

        for (let i = 0; i < places.length; i++) {

            if (!places[i].name) {

                return res.status(400).json({
                    success: false,
                    error: `Place ${i + 1} name is missing.`
                });

            }


            if (
                places[i].exploreTime === undefined ||
                places[i].exploreTime <= 0
            ) {

                return res.status(400).json({
                    success: false,
                    error:
                        `Exploration time for Place ${i + 1} is invalid.`
                });

            }

        }


        // --------------------------------------
        // PREPARE TRIP DATA
        // --------------------------------------

        const tripData = {

            startLocation: startLocation,

            places: places,

            finalLocation: finalLocation,

            availableTime: availableTime

        };


        // --------------------------------------
        // DISPLAY DATA IN SERVER CONSOLE
        // --------------------------------------

        console.log("");
        console.log("================================");
        console.log("NEW TRAVEL PLAN");
        console.log("================================");

        console.log(
            "Starting Location:",
            startLocation
        );

        console.log(
            "Available Time:",
            availableTime,
            "hours"
        );

        console.log("Places:");

        places.forEach((place, index) => {

            console.log(
                `${index + 1}. ${place.name} - ` +
                `${place.exploreTime} hours`
            );

        });

        console.log(
            "Final Location:",
            finalLocation
        );

        console.log("================================");
        console.log("");


        // --------------------------------------
        // TEMPORARY RESPONSE
        // --------------------------------------
        //
        // Later we will replace this section
        // with:
        //
        // 1. GPS API
        // 2. Distance calculation
        // 3. Travel time calculation
        // 4. C++ Branch and Bound
        // 5. Final route
        //
        // --------------------------------------

        res.json({

            success: true,

            message:
                "Trip data received successfully.",

            trip: tripData,

            status:
                "Waiting for GPS API and C++ algorithm."

        });


    } catch (error) {

        console.error(
            "Server Error:",
            error
        );


        res.status(500).json({

            success: false,

            error:
                "Internal server error."

        });

    }

});


// ==========================================
// START SERVER
// ==========================================

app.listen(PORT, () => {

    console.log(
        `Smart Travel Planner backend running on port ${PORT}`
    );

});
