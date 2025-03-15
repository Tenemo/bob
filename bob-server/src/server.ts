import axios from 'axios';

const BOB_IP = '192.168.212.23';

const delay = (ms: number): Promise<void> =>
    new Promise((resolve) => setTimeout(resolve, ms));

const rotate = async ({
    motorIndex,
    degrees,
}: {
    motorIndex?: number;
    degrees: number;
}): Promise<void> => {
    try {
        if (motorIndex === undefined) {
            throw new Error('motorIndex is required');
        }
        console.log(`Rotating motor: ${motorIndex} to ${degrees}`);
        const response = await axios.post(`http://${BOB_IP}/rotate/`, {
            motorIndex: motorIndex?.toString(),
            degrees,
        });
        console.log('Response:', response.data);
    } catch (error: unknown) {
        if (error instanceof Error) {
            console.error('Error1:', error.message);
        } else {
            console.error('Error2:', error);
        }
    }
    await delay(200);
};
const move = async ({ type }: { type: string }): Promise<void> => {
    try {
        console.log(`Moving: ${type}`);
        const response = await axios.post(`http://${BOB_IP}/move/`, {
            type,
        });
        console.log('Response:', response.data);
    } catch (error: unknown) {
        if (error instanceof Error) {
            console.error('Error1:', error.message);
        } else {
            console.error('Error2:', error);
        }
    }
};

const moveLegForward = async ({
    topMotorIndex,
}: {
    topMotorIndex: number;
}): Promise<void> => {
    await rotate({ motorIndex: topMotorIndex - 6, degrees: 30 });
    await rotate({ motorIndex: topMotorIndex, degrees: 70 });
};

const moveLegBackward = async ({
    topMotorIndex,
}: {
    topMotorIndex: number;
}): Promise<void> => {
    await rotate({ motorIndex: topMotorIndex - 6, degrees: 60 });
    await rotate({ motorIndex: topMotorIndex, degrees: 110 });
};

const walk = async (): Promise<void> => {
    await rotate({ motorIndex: 0, degrees: 60 });
    await rotate({ motorIndex: 6, degrees: 110 });
    await rotate({ motorIndex: 2, degrees: 60 });
    await rotate({ motorIndex: 8, degrees: 110 });
    await rotate({ motorIndex: 4, degrees: 60 });
    await rotate({ motorIndex: 10, degrees: 110 });

    await rotate({ motorIndex: 1, degrees: 30 });
    await rotate({ motorIndex: 7, degrees: 70 });
    await rotate({ motorIndex: 3, degrees: 30 });
    await rotate({ motorIndex: 9, degrees: 70 });
    await rotate({ motorIndex: 5, degrees: 30 });
    await rotate({ motorIndex: 11, degrees: 70 });

    await rotate({ motorIndex: 0, degrees: 30 });
    await rotate({ motorIndex: 6, degrees: 70 });
    await rotate({ motorIndex: 2, degrees: 30 });
    await rotate({ motorIndex: 8, degrees: 70 });
    await rotate({ motorIndex: 4, degrees: 30 });
    await rotate({ motorIndex: 10, degrees: 70 });

    await rotate({ motorIndex: 1, degrees: 60 });
    await rotate({ motorIndex: 7, degrees: 110 });
    await rotate({ motorIndex: 3, degrees: 60 });
    await rotate({ motorIndex: 9, degrees: 110 });
    await rotate({ motorIndex: 5, degrees: 60 });
    await rotate({ motorIndex: 11, degrees: 110 });
};

const execute = async (): Promise<void> => {
    await move({ type: 'reset' });
    // await move({ type: 'standUp' });
    // await moveLegForward({ topMotorIndex: 6 });
    // await moveLegBackward({ topMotorIndex: 9 });
    // await moveLegForward({ topMotorIndex: 7 });
    // await moveLegBackward({ topMotorIndex: 10 });
    // await moveLegForward({ topMotorIndex: 8 });
    // await moveLegBackward({ topMotorIndex: 11 });

    // await moveLegForward({ topMotorIndex: 7 });
    // await moveLegForward({ topMotorIndex: 9 });
    // await moveLegForward({ topMotorIndex: 11 });

    // await moveLegBackward({ topMotorIndex: 7 });
    // await moveLegBackward({ topMotorIndex: 9 });
    // await moveLegBackward({ topMotorIndex: 11 });

    // await moveLegForward({ topMotorIndex: 7 });
    // await moveLegBackward({ topMotorIndex: 10 });
    // await moveLegBackward({ topMotorIndex: 6 });
    // await moveLegForward({ topMotorIndex: 11 });
    // await moveLegBackward({ topMotorIndex: 8 });
    // await moveLegForward({ topMotorIndex: 10 });
};
void execute();
