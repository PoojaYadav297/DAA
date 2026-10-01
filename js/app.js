document.addEventListener("DOMContentLoaded", () => {

    const form = document.getElementById("travelForm");

    const numberOfPlaces =
        document.getElementById("numberOfPlaces");

    const addPlacesButton =
        document.getElementById("addPlacesButton");

    const placesContainer =
        document.getElementById("placesContainer");

    const calculateButton =
        document.getElementById("calculateButton");

    const resultSection =
        document.getElementById("resultSection");


    // ==========================================
    // CREATE PLACE INPUTS
    // ==========================================

    addPlacesButton.addEventListener("click", () => {

        const count = parseInt(numberOfPlaces.value);


        if (isNaN(count) || count < 1 || count > 10) {

            alert("Please enter between 1 and 10 places.");

            return;
        }


        // Clear previous places
        placesContainer.innerHTML = "";


        // Heading
        const heading = document.createElement("h3");

        heading.textContent =
            "Enter Places and Exploration Time";

        placesContainer.appendChild(heading);


        // Create inputs
        for (let i = 1; i <= count; i++) {

            const placeBox =
                document.createElement("div");

            placeBox.className = "place-box";


            // Place name

            const placeLabel =
                document.createElement("label");

            placeLabel.textContent =
                `Place ${i}`;

            const placeInput =
                document.createElement("input");

            placeInput.type = "text";

            placeInput.id =
                `place${i}`;

            placeInput.className =
                "place-name";

            placeInput.placeholder =
                `Enter place ${i}`;

            placeInput.required = true;


            // Exploration time

            const timeLabel =
                document.createElement("label");

            timeLabel.textContent =
                "Exploration Time (hours)";


            const timeInput =
                document.createElement("input");

            timeInput.type = "number";

            timeInput.id =
                `exploreTime${i}`;

            timeInput.className =
                "explore-time";

            timeInput.placeholder =
                "Example: 1";

            timeInput.min = "0.1";

            timeInput.step = "0.1";

            timeInput.required = true;


            // Add elements

            placeBox.appendChild(placeLabel);

            placeBox.appendChild(placeInput);

            placeBox.appendChild(timeLabel);

            placeBox.appendChild(timeInput);


            placesContainer.appendChild(placeBox);
        }


        // Show calculate button

        calculateButton.style.display =
            "block";

    });


    // ==========================================
    // SUBMIT TRIP
    // ==========================================

    form.addEventListener("submit", async (event) => {

        event.preventDefault();


        const startLocation =
            document.getElementById(
                "startLocation"
            ).value.trim();


        const finalLocation =
            document.getElementById(
                "finalLocation"
            ).value.trim();


        const availableTime =
            parseFloat(
                document.getElementById(
                    "availableTime"
                ).value
            );


        const count =
            parseInt(
                numberOfPlaces.value
            );


        // ======================================
        // VALIDATION
        // ======================================

        if (!startLocation) {

            alert("Please enter the starting location.");

            return;
        }


        if (!finalLocation) {

            alert("Please enter the final location.");

            return;
        }


        if (
            isNaN(availableTime) ||
            availableTime <= 0
        ) {

            alert("Please enter valid available time.");

            return;
        }


        // ======================================
        // COLLECT PLACES
        // ======================================

        const places = [];


        for (let i = 1; i <= count; i++) {

            const name =
                document.getElementById(
                    `place${i}`
                ).value.trim();


            const exploreTime =
                parseFloat(
                    document.getElementById(
                        `exploreTime${i}`
                    ).value
                );


            if (!name) {

                alert(
                    `Please enter Place ${i}.`
                );

                return;
            }


            if (
                isNaN(exploreTime) ||
                exploreTime <= 0
            ) {

                alert(
                    `Please enter valid exploration time for Place ${i}.`
                );

                return;
            }


            places.push({

                name: name,

                exploreTime: exploreTime

            });

        }


        // ======================================
        // DATA SENT TO BACKEND
        // ======================================

        const tripData = {

            startLocation: startLocation,

            places: places,

            finalLocation: finalLocation,

            availableTime: availableTime

        };


        console.log(
            "Trip Data:",
            tripData
        );


        // ======================================
        // CONNECT TO BACKEND
        // ======================================

        try {

            const response =
                await fetch(
                    "http://localhost:3000/api/plan-trip",
                    {
                        method: "POST",

                        headers: {
                            "Content-Type":
                                "application/json"
                        },

                        body:
                            JSON.stringify(
                                tripData
                            )
                    }
                );


            if (!response.ok) {

                throw new Error(
                    "Unable to create travel plan."
                );

            }


            const result =
                await response.json();


            console.log(
                "Backend Result:",
                result
            );


            displayResult(result);


        } catch (error) {

            console.error(error);


            alert(
                "Backend is not connected yet. Your trip data was prepared successfully."
            );

        }

    });


    // ==========================================
    // DISPLAY RESULT
    // ==========================================

    function displayResult(result) {

        resultSection.style.display =
            "block";


        const selectedPlaces =
            document.getElementById(
                "selectedPlaces"
            );


        const skippedPlaces =
            document.getElementById(
                "skippedPlaces"
            );


        const route =
            document.getElementById(
                "route"
            );


        const totalDistance =
            document.getElementById(
                "totalDistance"
            );


        const travelTime =
            document.getElementById(
                "travelTime"
            );


        const explorationTime =
            document.getElementById(
                "explorationTime"
            );


        const totalTime =
            document.getElementById(
                "totalTime"
            );


        const remainingTime =
            document.getElementById(
                "remainingTime"
            );


        // Selected places

        if (Array.isArray(result.selectedPlaces)) {

            selectedPlaces.textContent =
                result.selectedPlaces.join(" → ");

        }


        // Skipped places

        if (Array.isArray(result.skippedPlaces)) {

            skippedPlaces.textContent =
                result.skippedPlaces.join(", ");

        }


        // Route

        if (Array.isArray(result.route)) {

            route.textContent =
                result.route.join(" → ");

        }


        // Distance

        if (result.totalDistance !== undefined) {

            totalDistance.textContent =
                `${result.totalDistance} km`;

        }


        // Travel time

        if (result.travelTime !== undefined) {

            travelTime.textContent =
                formatTime(
                    result.travelTime
                );

        }


        // Exploration time

        if (result.explorationTime !== undefined) {

            explorationTime.textContent =
                formatTime(
                    result.explorationTime
                );

        }


        // Total time

        if (result.totalTime !== undefined) {

            totalTime.textContent =
                formatTime(
                    result.totalTime
                );

        }


        // Remaining time

        if (result.remainingTime !== undefined) {

            remainingTime.textContent =
                formatTime(
                    result.remainingTime
                );

        }


        // Visualization

        if (
            typeof window
                .showBranchAndBoundVisualization
            === "function"
        ) {

            window.showBranchAndBoundVisualization(
                result.searchTree || []
            );

        }

    }


    // ==========================================
    // FORMAT TIME
    // ==========================================

    function formatTime(hours) {

        const totalMinutes =
            Math.round(hours * 60);


        const hoursPart =
            Math.floor(
                totalMinutes / 60
            );


        const minutesPart =
            totalMinutes % 60;


        if (hoursPart === 0) {

            return `${minutesPart} min`;

        }


        if (minutesPart === 0) {

            return `${hoursPart} hr`;

        }


        return `${hoursPart} hr ${minutesPart} min`;

    }

});
