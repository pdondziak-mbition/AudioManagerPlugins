/**************************************************************************//**
 * @file  CAmConfigurationReader.h
 *
 * Permanently provides the configuration details of the system topology
 * (domains, sources, sinks, classes and properties) as well as behavior
 * according to business logic as defined in the assigned configuration
 * XML file, which is parsed on first usage.
 *
 * @component{AudioManager Generic Controller}
 *
 * @authors Toshiaki Isogai <tisogai@jp.adit-jv.com>,\n
 *          Kapildev Patel  <kpatel@jp.adit-jv.com>,\n
 *          Prashant Jain   <pjain@jp.adit-jv.com>,\n
 *          Martin Koch     <mkoch@de.adit-jv.com>
 *
 * @copyright (c) 2015 - 2020 Advanced Driver Information Technology.\n
 * This code is developed by Advanced Driver Information Technology.\n
 * Copyright of Advanced Driver Information Technology, Bosch, and DENSO.\n
 * All rights reserved.
 *
 *//**************************************************************************/

#ifndef GC_CONFIGURATIONREADER_H_
#define GC_CONFIGURATIONREADER_H_

#include "CAmXmlConfigParser.h"

namespace am {
namespace gc {

/**************************************************************************//**
 * @class CAmConfigurationReader
 * @copydoc CAmConfigurationReader.h
 */
class CAmConfigurationReader
{
public:
    /**
     * @brief  Singleton access function
     * @return Reference to permanent instance, initialized from assigned
     *         configuration file on first usage
     */
    static CAmConfigurationReader &instance(void);

    /**
     * @brief  (Re-)populate memorized configuration items from XML file
     * 
     *         Will be invoked from constructor. Public for testing purpose.
     */
     void reload(void);

    /**
     * @brief It is API providing the interface to get the list of sources as defined in configuration
     * @param listSources: list in which sources will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListSources(std::vector<gc_Source_s > &listSources);

    /**
     * @brief It is API providing the interface to get the list of sink as defined in configuration
     * @param listSinks: list in which sinks will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListSinks(std::vector<gc_Sink_s > &listSinks);

    /**
     * @brief It is API providing the interface to get the list of gateway as defined in configuration
     * @param listGateways: list in which gateways will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListGateways(std::vector<gc_Gateway_s > &listGateways);

    /**
     * @brief It is API providing the interface to get the list of domains as defined in configuration
     * @param listDomains: list in which domains will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListDomains(std::vector<gc_Domain_s > &listDomains);

    /**
     * @brief It is API providing the interface to get the list of classes as defined in configuration
     * @param listClasses: list in which classes will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListClasses(std::vector<gc_Class_s > &listClasses);

    /**
     * @brief It is API providing the interface to get the list of polices as defined in configuration for given trigger
     * @param trigger: trigger for which policy need to be retrieved
     *        listPolicySets: list in which polices will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getListProcess(const gc_Trigger_e trigger,
        std::vector<gc_Process_s > &listProcesses);

    /**
     * @brief It is API providing the interface to get the gateway information by name as defined in configuration
     * @param elementName: name of the gateway element
     *        gatewayInstance: variable in which information will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getElementByName(const std::string &elementName, gc_Gateway_s &gatewayInstance);

    /**
     * @brief It is API providing the interface to get the source information by name as defined in configuration
     * @param elementName: name of the source element
     *        sourceInstance: variable in which information will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getElementByName(const std::string &elementName, gc_Source_s &sourceInstance);

    /**
     * @brief It is API providing the interface to get the sink information by name as defined in configuration
     * @param elementName: name of the sink element
     *        sinkInstance: variable in which information will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getElementByName(const std::string &elementName, gc_Sink_s &sinkInstance);

    /**
     * @brief It is API providing the interface to get the domain information by name as defined in configuration
     * @param elementName: name of the sink element
     *        domainInstance: variable in which information will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getElementByName(const std::string &elementName, gc_Domain_s &domainInstance);

    /**
     * @brief It is API providing the interface to get the class information by name as defined in configuration
     * @param elementName: name of the sink element
     *        classInstance: variable in which information will be returned
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    am_Error_e getElementByName(const std::string &elementName, gc_Class_s &classInstance);

    /**
     * @brief It is API providing the interface to get the list of system property as defined in configuration
     * @param listSystemProperties: list in which properties will be returned
     * @return none
     */
    void getListSystemProperties(std::vector<gc_SystemProperty_s > &listSystemProperties);

    /// helper to access configured scale conversion maps
    void populateScalingMap(std::map<float, float > &outMap, const std::string &valuePairString);

private:
    CAmConfigurationReader();

    // prohibit usage of assignment operator and copy constructor
    CAmConfigurationReader(const CAmConfigurationReader &) = delete;
    CAmConfigurationReader &operator=(const CAmConfigurationReader &) = delete;

    /**
     * @brief It is the internal function use to prepare the sink set. Domain ID, class ID, Properties are stored in sink structure as defined in configuration
     * @param none
     * @return none
     */
    void _prepareSinkMap(gc_Configuration_s *pConfiguration);

    /**
     * @brief It is the internal function use to prepare the source set. Domain ID, class ID, Properties are stored in source structure as defined in configuration
     * @param none
     * @return none
     */
    void _prepareSourceMap(gc_Configuration_s *pConfiguration);

    /**
     * @brief It is the internal function use to prepare the gateway set. source and sink ID are stored of gateway are stored.
     * @param none
     * @return none
     */
    void _prepareGatewayMap(gc_Configuration_s *pConfiguration);
    void _prepareDomainMap(gc_Configuration_s *pConfiguration);
    void _prepareClassMap(gc_Configuration_s *pConfiguration);

    /**
     * @brief It is the internal template function use to get the element list from configuration
     * @param listElements: list of elements to returned
     *        mapElements: map of element
     * @return true on success
     *         false on error
     */
    template <class Telement>
    am_Error_e _getListElements(std::vector<Telement > &listElements,
        std::map<std::string, Telement > &mapElements);

    /**
     * @brief It is the internal template function use to get the element by name
     * @param name: name of the element
     *        element_p: instance of elements to returned
     *        listElements: list of element to be searched
     * @return E_OK on success
     *         E_UNKNOWN on error
     */
    template <class Telement>
    am_Error_e _getElementByName(const std::string &name, Telement &element_p,
        std::map<std::string, Telement > &mapElements);

    /**
     * @brief It is the internal function use to prepare the source and sink element set
     * @param itListElement: iterator of element type which need to be prepared
     * @return none
     */
    template <class T>
    void _prepareElementSet(gc_Configuration_s *pConfiguration, T &itListElement);
    static bool _policysorting(gc_Process_s i, gc_Process_s j);

    std::map<std::string, gc_Source_s >  mMapSources;
    std::map<std::string, gc_Sink_s >    mMapSinks;
    std::map<std::string, gc_Gateway_s > mMapGateways;
    std::map<std::string, gc_Domain_s >  mMapDomains;
    std::map<std::string, gc_Class_s >   mMapClasses;
    std::vector<gc_Policy_s >            mListPolicies;
    std::vector<gc_SystemProperty_s >    mListSystemProperties;
    std::map<std::string, std::map<float, float> > mMapScaleConversions;
};

}
}
#endif /* GC_CONFIGURATIONREADER_H_*/
