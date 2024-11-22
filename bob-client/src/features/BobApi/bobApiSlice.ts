import { createApi, fetchBaseQuery } from '@reduxjs/toolkit/query/react';
export type HealthcheckResponse = {
    status: string;
};

export const bobApiSlice = createApi({
    reducerPath: 'bobApi',
    baseQuery: fetchBaseQuery({
        baseUrl: '',
    }),
    endpoints: (builder) => ({
        /**
         * Health check query.
         */
        healthcheck: builder.query<HealthcheckResponse, void>({
            query: () => `health-check`,
        }),
        /**
         * Capture photo query.
         */
        capture: builder.query<Blob, void>({
            query: () => ({
                url: `capture`,
                method: 'GET',
                // Explicitly type the response parameter
                responseHandler: (response: Response) => response.blob(),
            }),
        }),
    }),
});

export const { useHealthcheckQuery, useCaptureQuery } = bobApiSlice;
