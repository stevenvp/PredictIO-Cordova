//
//  PredictIO.h
//  predict.io-sdk
//
//  Created by Zee on 28/02/2013.
//  Copyright (c) 2016 predict.io by ParkTAG GmbH. All rights reserved.
//  SDK Version 3.2.0

#import <Foundation/Foundation.h>
#import "PIOTripSegment.h"
#import "PIOZone.h"

@protocol PredictIODelegate;

@interface PredictIO : NSObject

@property (nonatomic, weak) id <PredictIODelegate> delegate;
@property (nonatomic, strong) NSString *apiKey;
@property (nonatomic, strong) NSString *version;

/* Home zone of the user detected by PredicIO */
@property (nonatomic, strong) PIOZone *homeZone;

/* Work zone of the user detected by PredicIO */
@property (nonatomic, strong) PIOZone *workZone;

+ (PredictIO *)sharedInstance;

/* This starts predict.io if delegate and API-Key are set, otherwise it returns an Error
 * @param handler: The argument to the completion handler block is an error object that contains
 * the description of the error. If predict.io is started successfully, then error object is nil
 */
- (void)startWithCompletionHandler:(void(^)(NSError *error))handler;

/*
 * Stop predict.io
 */
- (void)stop;

/*
 * Manually activate GPS for short period of time
 */
- (void)kickStartGPS;

/* This method returns the status of predict.io, PredictIOStatus i.e. if it is active or not
 * Discussion: PredictIOStatus represents the current predict.io state
 * PredictIOStatusActive: predict.io is in a working, active state
 * PredictIOStatusLocationServicesDisabled: predict.io is not in a working state as the location services are disabled
 * PredictIOStatusInsufficientPermission: predict.io is not in a working state as the permissions to use location services are not provided by the user
 * PredictIOStatusInActive: predict.io has not been started. It is in inactive state
 */
- (PredictIOStatus)status;

/*
 * An alphanumeric string that uniquely identifies a device to the predict.io
 */
- (NSString *)deviceIdentifier;

/* Set custom parameters which can then be sent to a user defined webhook url
 * @param key:   Key to identify a custom parameter value
 * @param value: Custom parameter value
 */
- (void)setCustomParameter:(NSString *)key andValue:(NSString *)value;

/* Set a webhook url where all the detected events can then be forwarded along with the custom parameters.
 * This webhook will not support additional authentication. So any additional validation of legitimate requests must take place.
 * @param url: The webhook url
 */
- (void)setWebhookURL:(NSString *)url;

/*
 * Clear historic zone data which is used to predict different zones like home or work zones
 */
- (void)clearZoneHistory;

@end

@protocol PredictIODelegate <NSObject>

@optional

/* This method is invoked when predict.io detects that the user is about to depart
 * from his location and is approaching to his vehicle
 * @param tripSegment: PIOTripSegment contains details about departing event
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  departureZone: Departure zone
 */
- (void)departing:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io detects that the user has just departed
 * from his location and have started a new trip
 * @param tripSegment: PIOTripSegment contains details about departure event
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  departureTime: Time of departure
 *  departureZone: Departure zone
 */
- (void)departed:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io is unable to validate the last departure event.
 * This can be due to invalid data received from sensors or the trip amplitude.
 * i.e. If the trip takes less than 2 minutes or the distance travelled is less than 1km
 * @param tripSegment: PIOTripSegment have departure canceled event information
 * @discussion: At this point only following properties will be populated,
 *  UUID: Unique ID for a trip segment, e.g. to link departure and departure canceled events
 *  departureLocation: The Location from where the user departed
 *  departureTime: Start time of the trip
 *  transportationMode: Mode of transportation
 *  departureZone: Departure zone
 */
- (void)canceledDeparture:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io detects transportation mode
 * @param tripSegment: PIOTripSegment contains details about user's transportation mode
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  departureTime: Time of departure
 *  transportationMode: Mode of transportation
 */
- (void)detectedTransportationMode:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io suspects that the user has just arrived
 * at his location and have ended a trip
 * Most of the time it is followed by a confirmed arrivedAtLocation event
 * If you need only confirmed arrival events, use arrivedAtLocation method (below) instead
 * @param tripSegment: PIOTripSegment contains details about arrival suspected event
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  arrivalLocation: The Location where the user arrived and ended the trip
 *  departureTime: Time of departure
 *  arrivalTime: Time of arrival
 *  transportationMode: Mode of transportation
 */
- (void)suspectedArrival:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io detects that the user has just arrived at destination
 * @param tripSegment: PIOTripSegment contains details about arrival event
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  arrivalLocation: The Location where the user arrived and ended the trip
 *  departureTime: Time of departure
 *  arrivalTime: Time of arrival
 *  transportationMode: Mode of transportation
 *  departureZone: Departure zone
 *  arrivalZone: Arrival Zone
 */
- (void)arrived:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io detects that the user is searching for a
 * parking space at a specific location
 * @param location: The Location where predict.io identifies that user is searching for a parking space
 */
- (void)searchingInPerimeter:(CLLocation *)searchingLocation;

/* This method is invoked after few minutes of arriving at the destination and detects if the user is stationary or not
 * @param tripSegment: PIOTripSegment contains details about stationary after arrival
 * @discussion: The following properties are populated currently:
 *  UUID: Unique ID for a trip segment, e.g. to link departure and arrival events
 *  departureLocation: The Location from where the user departed
 *  arrivalLocation: The Location where the user arrived and ended the trip
 *  departureTime: Time of departure
 *  arrivalTime: Time of arrival
 *  transportationMode: Mode of transportation
 *  departureZone: Departure zone
 *  arrivalZone: Arrival Zone
 *  stationary: User activity status as stationary or not
 */
- (void)beingStationaryAfterArrival:(PIOTripSegment *)tripSegment;

/* This method is invoked when predict.io detects that the user has traveled by air plane and
 * just arrived at destination, this event is independent of usual vehicle trip detection and
 * will not have predecessor departed event
 * @param tripSegment: PIOTripSegment contains details about traveled by air plane event
 * @discussion: The following properties are populated currently
 *  UUID: Unique ID for a trip segment
 *  departureLocation: The Location from where the user started journey
 *  arrivalLocation: The Location where the user arrived and ended the journey
 *  departureTime: Start time of journey
 *  arrivalTime: Stop time of journey
 */
- (void)traveledByAirplane:(PIOTripSegment *)tripSegment;

/* This is invoked when new location information is received from location services
 * Implemented this method if you need raw GPS data, instead of creating new location manager
 * Since, it is not recommended to use multiple location managers in a single app
 * @param location: New location
 */
- (void)didUpdateLocation:(CLLocation *)location;

@end

/*
 * These notifications are sent out after the equivalent delegate message is called
 */
FOUNDATION_EXPORT NSString *const PIODepartingNotification;
FOUNDATION_EXPORT NSString *const PIODepartedNotification;
FOUNDATION_EXPORT NSString *const PIOCanceledDepartureNotification;
FOUNDATION_EXPORT NSString *const PIOSuspectedArrivalNotification;
FOUNDATION_EXPORT NSString *const PIOArrivedNotification;
FOUNDATION_EXPORT NSString *const PIOSearchingParkingNotification;
FOUNDATION_EXPORT NSString *const PIODetectedTransportationModeNotification;
FOUNDATION_EXPORT NSString *const PIOBeingStationaryAfterArrivalNotification;
FOUNDATION_EXPORT NSString *const PIOTraveledByAirplaneNotification;
